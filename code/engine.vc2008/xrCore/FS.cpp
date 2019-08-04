#include "stdafx.h"
#pragma hdrstop

#include "fs_internal.h"

#pragma warning(disable:4995 4267)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

//////////////////////////////////////////////////////////////////////
// Tools
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
XRCORE_API void createPath(LPCSTR path, bool bIsFileName /*= false*/, bool bIsAbsolute /*= false*/)
{
	xr_string targetPath = path;
	xr_string::FixSlashes(targetPath);
	xr_vector<xr_string> pathTokens = targetPath.Split('\\');
	if (pathTokens.size() < 2) return;

	// if we have filename, we should omit it
	int pathTokensSize = bIsFileName ? pathTokens.size() : pathTokens.size() + 1;

	int i = 1;
	if (bIsAbsolute) i++;
	for (; i < pathTokensSize; i++)
	{
		xr_string targetDir = xr_string::Join(pathTokens.begin(), pathTokens.begin() + i, '\\');

		WIN32_FIND_DATAA findData = { 0 };
		HANDLE hFind = FindFirstFileA(targetDir.c_str(), &findData);

		if (!hFind || hFind == INVALID_HANDLE_VALUE)
		{
			// if this path don't include any path - create new directory
			BOOL bCreateDir = CreateDirectoryA(targetDir.c_str(), nullptr);
			VERIFY(bCreateDir);
		}
		else
		{
			// get type of path
			bool bDir = ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0);
			FindClose(hFind);

			// the path can't be be file, so we delete it
			if (!bDir)
			{
				DeleteFileA(targetDir.c_str());
			}
		}
	}
}

static bool open_internal(const char* fn, HANDLE& handle) 
{
	handle = CreateFileA(fn, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	return handle != INVALID_HANDLE_VALUE;
}

bool file_handle_internal(const char* file_name, size_t& size, HANDLE& file_handle) 
{
    if (!open_internal(file_name, file_handle)) 
	{
		return false;
    }

	LARGE_INTEGER largeInt = {};
	GetFileSizeEx(file_handle, &largeInt);	
	size = largeInt.QuadPart;
    return true;
}

void* FileDownload(const char* file_name, HANDLE file_handle, size_t file_size) 
{
    void* buffer = Memory.mem_alloc(file_size);

	DWORD r_bytes = 0;		//_read(file_handle, buffer, file_size);
	ReadFile(file_handle, buffer, file_size, &r_bytes, nullptr);

    R_ASSERT3(file_size == static_cast<size_t>(r_bytes), "can't read from file : ", file_name);

	if (file_handle) { CloseHandle(file_handle); }
    return buffer;
}

void* FileDownload(const char* file_name, size_t* buffer_size) {
    HANDLE file_handle = INVALID_HANDLE_VALUE;
    R_ASSERT3(file_handle_internal(file_name, *buffer_size, file_handle),
              "can't open file : ", file_name);

    return FileDownload(file_name, file_handle, *buffer_size);
}

using MARK = std::array<char, 9>;

inline void mk_mark(MARK& M, const char* S) {
    strncpy_s(M.data(), sizeof(M), S, 8);
}

void FileCompress(const char* fn, const char* sign, void* data, const size_t size) {
	MARK M = {};
    mk_mark(M, sign);
	DWORD SizeOut = 0;

    HANDLE hFile = CreateFileA(fn, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    R_ASSERT2(hFile, fn);

	WriteFile(hFile, &M, sizeof(size_t), &SizeOut, nullptr);

	XRay::Compress::LZ::WriteLZ(hFile, data, (u32)size);
	CloseHandle(hFile);
}

void* FileDecompress(const char* fn, const char* sign, size_t* size) {
    MARK M, F;
    mk_mark(M, sign);
	DWORD SizeOut = 0;

    HANDLE hFile = CreateFileA(fn, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    R_ASSERT2(hFile, fn);

	ReadFile(hFile, &F, sizeof(size_t), &SizeOut, nullptr);

    if (strncmp(M.data(), F.data(), 8) != 0) {
        F[8] = 0;
        Msg("FATAL: signatures doesn't match, file(%s) / requested(%s)", F, sign);
    }
    R_ASSERT(strncmp(M.data(), F.data(), 8) == 0);

    void* ptr = nullptr;
	LARGE_INTEGER largeInt = {};
	GetFileSizeEx(hFile, &largeInt);	// don't use here GetFileSize, it can't open file < Page size (default - 64KB)

    const size_t SZ = XRay::Compress::LZ::ReadLZ(hFile, ptr, largeInt.LowPart - 8);

	CloseHandle(hFile);
    if (size)
        *size = SZ;
    return ptr;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// memory
CMemoryWriter::~CMemoryWriter() { xr_free(data); }

void CMemoryWriter::w(const void* ptr, const size_t count) {
    if (position + count > mem_size) {
        // reallocate
        if (mem_size == 0)
            mem_size = 128;
        while (mem_size <= (position + count))
            mem_size *= 2;
        if (!data)
            data = static_cast<u8*>(Memory.mem_alloc(mem_size));
        else
            data = static_cast<u8*>(Memory.mem_realloc(data, mem_size));
    }
    std::memcpy(data + position, ptr, count);
    position += count;
    if (position > file_size)
        file_size = position;
}

// static const u32 mb_sz = 0x1000000;
bool CMemoryWriter::save_to(const char* fn) {
    IWriter* F = FS.w_open(fn);
    if (F) {
        F->w(pointer(), size());
        FS.w_close(F);
        return true;
    }
    return false;
}

void IWriter::open_chunk(const u32 type) {
    w_u32(type);
    chunk_pos.push(tell());
    w_u32(0); // the place for 'size'
}

void IWriter::close_chunk() {
    VERIFY(!chunk_pos.empty());

    const size_t pos = tell();
    seek(chunk_pos.top());
    w_u32(pos - chunk_pos.top() - 4);
    seek(pos);
    chunk_pos.pop();
}

// returns size of currently opened chunk, 0 otherwise
size_t IWriter::chunk_size() {
    if (chunk_pos.empty())
        return 0;
    return tell() - chunk_pos.top() - 4;
}

void IWriter::w_compressed(void* ptr, const size_t count) {
    u8* dest = nullptr;
    unsigned dest_sz = 0;
	XRay::Compress::LZ::CompressLZ(&dest, &dest_sz, ptr, count);

    if (dest && dest_sz)
        w(dest, dest_sz);
    xr_free(dest);
}

void IWriter::w_chunk(const u32 type, void* data, const size_t size) {
    open_chunk(type);
    if (type & CFS_CompressMark)
        w_compressed(data, size);
    else
        w(data, size);
    close_chunk();
}

void IWriter::w_sdir(const Fvector& D) {
    Fvector C;
    float mag = D.magnitude();
    if (mag > EPS_S) {
        C.div(D, mag);
    } else {
        C.set(0, 0, 1);
        mag = 0;
    }
    w_dir(C);
    w_float(mag);
}

//---------------------------------------------------
// base stream
IReader* IReader::open_chunk(const u32 ID) 
{
    bool bCompressed;

    const size_t dwSize = find_chunk(ID, &bCompressed);
    if (dwSize != 0) {
        if (bCompressed) {
            u8* dest;
            unsigned dest_sz;
			XRay::Compress::LZ::DecompressLZ(&dest, &dest_sz, pointer(), dwSize);
            return new CTempReader(dest, dest_sz, tell() + dwSize);
        } else {
            return new IReader(pointer(), dwSize, tell() + dwSize);
        }
    } else
        return nullptr;
}

// TODO: imdex note: pay attention!
void IReader::close() { xr_delete(this); }

IReader* IReader::open_chunk_iterator(u32& ID, IReader* _prev) {
    if (!_prev) {
        // first
        rewind();
    } else {
        // next
        seek(_prev->iterpos);
        _prev->close();
    }

    //	open
    if (elapsed() < 8)
        return nullptr;

    ID = r_u32();
    const size_t _size = r_u32();
    if (ID & CFS_CompressMark) {
        // compressed
        u8* dest;
        unsigned dest_sz;
		XRay::Compress::LZ::DecompressLZ(&dest, &dest_sz, pointer(), _size);
        return new CTempReader(dest, dest_sz, tell() + _size);
    } else {
        // normal
        return new IReader(pointer(), _size, tell() + _size);
    }
}
#include <imdexlib\fast_dynamic_cast.hpp>
void IReader::r(void* p, const size_t cnt) 
{
    VERIFY(Pos + cnt <= Size);
    std::memcpy(p, pointer(), cnt);
    advance(cnt);
#ifdef DEBUG
    if (imdexlib::fast_dynamic_cast<CFileReader*>(this) || imdexlib::fast_dynamic_cast<CVirtualFileReader*>(this)) 
	{
        FS.dwOpenCounter++;
    }
#endif
};

inline bool is_term(const char a) { return (a == 13) || (a == 10); }

size_t IReader::advance_term_string() {
    size_t sz = 0;
    while (!eof()) {
        Pos++;
        sz++;
        if (!eof() && is_term(data[Pos])) {
            while (!eof() && is_term(data[Pos]))
                Pos++;
            break;
        }
    }
    return sz;
}

void IReader::r_string(char* dest, const size_t tgt_sz) {
    char* src = data + Pos;
    const size_t sz = advance_term_string();
    R_ASSERT2(sz < (tgt_sz - 1), "Dest string less than needed.");

    strncpy_s(dest, tgt_sz, src, sz);
    dest[sz] = 0;
}

void IReader::r_string(xr_string& dest) {
    const char* src = data + Pos;
    const auto sz = advance_term_string();
    dest.assign(src, sz);
}

void IReader::r_stringZ(char* dest, const size_t tgt_sz) 
{
    const auto sz = std::strlen(data);
    R_ASSERT2(sz < tgt_sz, "Dest string less than needed.");
    while ((data[Pos] != 0) && !eof())
        *dest++ = data[Pos++];
    *dest = 0;
    Pos++;
}

void IReader::r_stringZ(shared_str& dest) {
    dest = data + Pos;
    Pos += dest.size() + 1;
}

void IReader::r_stringZ(xr_string& dest) {
    dest = data + Pos;
    Pos += int(dest.size() + 1);
}

void IReader::skip_stringZ() {
    char* src = data;
    while ((src[Pos] != 0) && !eof())
        Pos++;
    Pos++;
}

//---------------------------------------------------
// temp stream
CTempReader::~CTempReader()
{ 
	xr_free(data); 
};
//---------------------------------------------------
// pack stream
CPackReader::~CPackReader() 
{
    UnmapViewOfFile(base_address);
};
//---------------------------------------------------
// file stream
CFileReader::CFileReader(const char* name) 
{
    // TODO: pay attention!
    size_t sz = Size;
    data = static_cast<char*>(FileDownload(name, &sz));
    Size = sz;
    Pos = 0;
};

CFileReader::~CFileReader() 
{ 
	xr_free(data); 
};
//---------------------------------------------------
// compressed stream
CCompressedReader::CCompressedReader(const char* name, const char* sign) {
    size_t sz = Size;
    data = static_cast<char*>(FileDecompress(name, sign, &sz));
    Size = sz;
    Pos = 0;
}

CCompressedReader::~CCompressedReader() 
{ 
	xr_free(data);
};

CVirtualFileRW::CVirtualFileRW(const char* cFileName) 
{
    // Open the file
    hSrcFile = CreateFile(cFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, 0, nullptr);
    R_ASSERT3(hSrcFile != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));

	LARGE_INTEGER li = {};
	GetFileSizeEx(hSrcFile, &li);
    Size = static_cast<int>(li.QuadPart);
    R_ASSERT3(Size, cFileName, Debug.error2string(GetLastError()));

    hSrcMap = CreateFileMapping(hSrcFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
    R_ASSERT3(hSrcMap != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));

    data = static_cast<char*>(MapViewOfFile(hSrcMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    R_ASSERT3(data, cFileName, Debug.error2string(GetLastError()));
}

CVirtualFileRW::~CVirtualFileRW() 
{
    UnmapViewOfFile(data);
    CloseHandle(hSrcMap);
    CloseHandle(hSrcFile);
}

CVirtualFileReader::CVirtualFileReader(const char* cFileName)
{
    // Open the file
    hSrcFile = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                          OPEN_EXISTING, 0, nullptr);
    R_ASSERT3(hSrcFile != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));

	LARGE_INTEGER li = {};
	GetFileSizeEx(hSrcFile, &li);
	Size = static_cast<int>(li.QuadPart);
	R_ASSERT3(Size, cFileName, Debug.error2string(GetLastError()));

    hSrcMap = CreateFileMapping(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    R_ASSERT3(hSrcMap != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));

    data = static_cast<char*>(MapViewOfFile(hSrcMap, FILE_MAP_READ, 0, 0, 0));
    R_ASSERT3(data, cFileName, Debug.error2string(GetLastError()));
}

CVirtualFileReader::~CVirtualFileReader() 
{
    UnmapViewOfFile(data);
    CloseHandle(hSrcMap);
    CloseHandle(hSrcFile);
}

bool EFS_Utils::GetOpenName(const char* initial, xr_string& buffer, bool bMulti, const char* offset, int start_flt_ext)
{
	char buf[255 * 255]; //max files to select
	xr_strcpy(buf, buffer.c_str());

	const bool bRes = GetOpenNameInternal(initial, buf, sizeof(buf), bMulti, offset, start_flt_ext);

	if (bRes)
		buffer = (char*)buf;

	return bRes;
}


bool EFS_Utils::GetSaveName(const char* initial, xr_string& buffer, const char* offset, int start_flt_ext)
{
	string_path buf;
	xr_strcpy(buf, sizeof(buf), buffer.c_str());
	const bool bRes = GetSaveName(initial, buf, offset, start_flt_ext);
	if (bRes)
		buffer = buf;

	return bRes;
}
//----------------------------------------------------

void EFS_Utils::MarkFile(const char* fn, bool bDeleteSource)
{
	xr_string ext = strext(fn);
	ext.insert(1, "~");
	xr_string backup_fn = EFS.ChangeFileExt(fn, ext.c_str());

	if (bDeleteSource)
		FS.file_rename(fn, backup_fn.c_str(), true);
	else
		FS.file_copy(fn, backup_fn.c_str());
}

xr_string EFS_Utils::AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name)
{
	string1024 nm;
	xr_strcpy(nm, tex_name.c_str());
	tex_name = AppendFolderToName(nm, sizeof(nm), depth, full_name);
	return tex_name;
}

CMemoryReader::CMemoryReader(void* InData, const int InSize, const int iterpos /*= 0*/)
{
	this->data = (char*)InData;
	this->Pos = 0;
	this->Size = InSize;
	this->iterpos = iterpos;
}
