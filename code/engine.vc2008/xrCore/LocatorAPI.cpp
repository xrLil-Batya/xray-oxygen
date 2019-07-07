// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ppl.h>
#pragma warning(default:4995)

#include "FS_internal.h"
#include "stream_reader.h"
#include "file_stream_reader.h"


const u32 BIG_FILE_READER_WINDOW_SIZE = 1024 * 1024;

#	pragma warning(push)
#	pragma warning(disable:4995)
#	include <malloc.h>
#	pragma warning(pop)

CLocatorAPI*		xr_FS = nullptr;

#define FSLTX "fsgame.ltx"

// Xottab-DUTY
constexpr u32 VFS_STANDARD_FILE = std::numeric_limits<u32>::max();

//#TODO: Make a part of CLocatorAPI class later
std::experimental::filesystem::path fsRoot;

bool file_handle_internal(const char* file_name, size_t& size, HANDLE& file_handle);
void* FileDownload(const char* file_name, size_t* buffer_size);


struct _open_file
{
	union {
		IReader*		_reader;
		CStreamReader*	_stream_reader;
	};
	shared_str			_fn;
	u32					_used;
};

template <typename T>
struct eq_pointer;

template <> struct eq_pointer<IReader> {
	IReader* _val;
	eq_pointer(IReader* p) :_val(p) {}
	bool operator () (_open_file& itm) {
		return (_val == itm._reader);
	}
};

template <> struct eq_pointer<CStreamReader> {
	CStreamReader* _val;
	eq_pointer(CStreamReader* p) :_val(p) {}
	bool operator () (_open_file& itm) {
		return (_val == itm._stream_reader);
	}
};

struct eq_fname_free {
	shared_str _val;
	eq_fname_free(shared_str s) { _val = s; }
	bool operator () (_open_file& itm) {
		return (_val == itm._fn && itm._reader == nullptr);
	}
};

struct eq_fname_check {
	shared_str _val;
	eq_fname_check(shared_str s) { _val = s; }
	bool operator () (_open_file& itm) {
		return (_val == itm._fn && itm._reader != nullptr);
	}
};

XRCORE_API xr_vector<_open_file>	g_open_files;

void _check_open_file(const shared_str& _fname)
{
	xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_check(_fname));
	if (it != g_open_files.end())
		Msg("file opened at least twice %s", _fname.c_str());
}

_open_file& find_free_item(const shared_str& _fname)
{
	xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_free(_fname));
	if (it == g_open_files.end())
	{
		g_open_files.resize(g_open_files.size() + 1);
		_open_file& _of = g_open_files.back();
		_of._fn = _fname;
		_of._used = 0;
		return					_of;
	}
	return *it;
}

void setup_reader(CStreamReader* _r, _open_file& _of)
{
	_of._stream_reader = _r;
}

void setup_reader(IReader* _r, _open_file& _of)
{
	_of._reader = _r;
}

template <typename T>
void _register_open_file(T* _r, const char* _fname)
{
	shared_str f = _fname;
	_check_open_file(f);

	_open_file& _of = find_free_item(_fname);
	setup_reader(_r, _of);
	_of._used += 1;
}

template <typename T>
void _unregister_open_file(T* _r)
{
	xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_pointer<T>(_r));
	VERIFY(it != g_open_files.end());
	_open_file&	_of = *it;
	_of._reader = nullptr;
}

XRCORE_API void _dump_open_files(int mode)
{
	bool bShow = false;
	if (mode == 1)
	{
		for (_open_file& _of : g_open_files)
		{
			if (_of._reader != nullptr)
			{
				if (!bShow)
					Log("----opened files");

				bShow = true;
				Msg("[%d] fname:%s", _of._used, _of._fn.c_str());
			}
		}
	}
	else
	{
		Log("----un-used");
		for (_open_file& _of : g_open_files)
		{
			if (!_of._reader)
				Msg("[%d] fname:%s", _of._used, _of._fn.c_str());
		}
	}
	if (bShow)
		Msg("----total count = %zu", g_open_files.size());
}

CLocatorAPI::CLocatorAPI()
{
	m_Flags.zero();
	// get page size
	SYSTEM_INFO			sys_inf;
	GetSystemInfo(&sys_inf);
	dwAllocGranularity = sys_inf.dwAllocationGranularity;
	m_iLockRescan = 0;
	dwOpenCounter = 0;
}

CLocatorAPI::~CLocatorAPI()
{
	VERIFY(0 == m_iLockRescan);
	_dump_open_files(1);
}

void CLocatorAPI::Register(const char* name, u32 vfs, u32 crc, u32 ptr, u32 size_real, u32 size_compressed, u32 modif)
{
	string256			temp_file_name;
	xr_strcpy(temp_file_name, sizeof(temp_file_name), name);
	xr_strlwr(temp_file_name);

	// Register file
	file desc;
	desc.name = temp_file_name;
	desc.vfs = vfs;
	desc.crc = crc;
	desc.ptr = ptr;
	desc.size_real = size_real;
	desc.size_compressed = size_compressed;
	desc.modif = modif & (~u32(0x3));
	//	Msg("registering file %s - %d", name, size_real);
	//	if file already exist - update info
	files_it			I = m_files.find(desc);
	if (I != m_files.end()) {
		//.		Msg("-- file already scanned [%s]", I->name);
		desc.name = I->name;

		// sad but true, performance option
		// correct way is to erase and then insert new record:
		const_cast<file&>(*I) = desc;
		return;
	}
	else {
		desc.name = xr_strdup(desc.name);
	}

	// otherwise insert file
	m_files.insert(desc);

	// Try to register folder(s)
	string_path			temp;
	xr_strcpy(temp, sizeof(temp), desc.name);
	string_path			path;
	string_path			folder;
	while (temp[0])
	{
		_splitpath(temp, path, folder, nullptr, nullptr);
		xr_strcat(path, folder);
		if (!exist(path))
		{
			desc.name = xr_strdup(path);
			desc.vfs = VFS_STANDARD_FILE;
			desc.ptr = 0;
			desc.size_real = 0;
			desc.size_compressed = 0;
			desc.modif = u32(-1);

			R_ASSERT(m_files.insert(desc).second);
		}
		xr_strcpy(temp, sizeof(temp), folder);
		if (xr_strlen(temp))		temp[xr_strlen(temp) - 1] = 0;
	}
}

IReader* open_chunk(void* ptr, u32 ID)
{
	BOOL			res;
	u32				dwType, dwSize;
	DWORD			read_byte;
	u32 pt = SetFilePointer(ptr, 0, nullptr, FILE_BEGIN); VERIFY(pt != INVALID_SET_FILE_POINTER);
	while (true) {
		res = ReadFile(ptr, &dwType, 4, &read_byte, nullptr);
		if (read_byte == 0)
			return nullptr;

		res = ReadFile(ptr, &dwSize, 4, &read_byte, nullptr);
		if (read_byte == 0)
			return nullptr;

		if ((dwType&(~CFS_CompressMark)) == ID) {
			u8* src_data = xr_alloc<u8>(dwSize);
			res = ReadFile(ptr, src_data, dwSize, &read_byte, nullptr); VERIFY(res && (read_byte == dwSize));
			if (dwType&CFS_CompressMark) {
				BYTE*			dest;
				unsigned		dest_sz;
				XRay::Compress::LZ::DecompressLZ(&dest, &dest_sz, src_data, dwSize);
				xr_free(src_data);
				return new CTempReader(dest, dest_sz, 0);
			}
			else
				return new CTempReader(src_data, dwSize, 0);
		}
		else {
			pt = SetFilePointer(ptr, dwSize, nullptr, FILE_CURRENT);
			if (pt == INVALID_SET_FILE_POINTER) return nullptr;
		}
	}
	return nullptr;
};

#ifndef PVS_STUDIO // IReader* hdr: is not a memory leak
void CLocatorAPI::LoadArchive(archive& A, const char* entrypoint)
{
	// Create base path
	string_path fs_entry_point;
	fs_entry_point[0] = 0;
	if (A.header)
	{
		shared_str read_path = A.header->r_string("header", "entry_point");
		if (0 == stricmp(read_path.c_str(), "gamedata"))
		{
			read_path = "$fs_root$";
			auto P = pathes.find(read_path.c_str());
			if (P != pathes.end())
			{
				FS_Path* root = P->second;
				xr_strcpy(fs_entry_point, sizeof(fs_entry_point), root->m_Path);
			}
			xr_strcat(fs_entry_point, "gamedata\\");
		}
		else
		{
			string256			alias_name;
			alias_name[0] = 0;
			R_ASSERT2(*read_path.c_str() == '$', read_path.c_str());

			int count = sscanf(read_path.c_str(), "%[^\\]s", alias_name);
			R_ASSERT2(count == 1, read_path.c_str());

			auto P = pathes.find(alias_name);

			if (P != pathes.end())
			{
				FS_Path* root = P->second;
				xr_strcpy(fs_entry_point, sizeof(fs_entry_point), root->m_Path);
			}
			xr_strcat(fs_entry_point, sizeof(fs_entry_point), read_path.c_str() + xr_strlen(alias_name) + 1);
		}

	}
	else
	{
		R_ASSERT2(0, "unsupported");
		xr_strcpy(fs_entry_point, sizeof(fs_entry_point), A.path.c_str());
		if (strext(fs_entry_point))
			*strext(fs_entry_point) = 0;
	}
	if (entrypoint)
		xr_strcpy(fs_entry_point, sizeof(fs_entry_point), entrypoint);

	// Read FileSystem
	A.open();
	IReader* hdr = open_chunk(A.hSrcFile, 1);
	R_ASSERT(hdr);
	RStringVec			fv;
	while (!hdr->eof())
	{
		string_path		name, full;
		string1024		buffer_start;
		u16				buffer_size = hdr->r_u16();
		VERIFY(buffer_size < sizeof(name) + 4 * sizeof(u32));
		VERIFY(buffer_size < sizeof(buffer_start));
		u8				*buffer = (u8*)&*buffer_start;
		hdr->r(buffer, buffer_size);

		u32 size_real = *(u32*)buffer;
		buffer += sizeof(size_real);

		u32 size_compr = *(u32*)buffer;
		buffer += sizeof(size_compr);

		u32 crc = *(u32*)buffer;
		buffer += sizeof(crc);

		std::size_t				name_length = buffer_size - 4 * sizeof(u32);
		std::memcpy(name, buffer, name_length);
		name[name_length] = 0;
		buffer += buffer_size - 4 * sizeof(u32);

		u32 ptr = *(u32*)buffer;
		buffer += sizeof(ptr);

		xr_strconcat(full, fs_entry_point, name);

		Register(full, A.vfs_idx, crc, ptr, size_real, size_compr, 0);
	}
	hdr->close();
}
#endif

void CLocatorAPI::archive::open()
{
	// Open the file
	if (hSrcFile && hSrcMap)
		return;

	hSrcFile = CreateFile(*path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	R_ASSERT(hSrcFile != INVALID_HANDLE_VALUE);
	hSrcMap = CreateFileMapping(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	R_ASSERT(hSrcMap != INVALID_HANDLE_VALUE);
	size = GetFileSize(hSrcFile, nullptr);
	R_ASSERT(size > 0);
}

void CLocatorAPI::archive::close()
{
	CloseHandle(hSrcMap);
	hSrcMap = nullptr;
	CloseHandle(hSrcFile);
	hSrcFile = nullptr;
}

void CLocatorAPI::ProcessArchive(const char* _path)
{
	// find existing archive
	shared_str path = _path;

	for (const auto& it : m_archives)
		if (it.path == path)
			return;

	m_archives.emplace_back();
	archive& A = m_archives.back();
	A.vfs_idx = (u32)m_archives.size() - 1;
	A.path = path;

	A.open();

	// Read header
	BOOL bProcessArchiveLoading = TRUE;

	IReader* hdr = open_chunk(A.hSrcFile, CFS_HeaderChunkID);
	if (hdr)
	{
		A.header = new CInifile(hdr, "archive_header");
		hdr->close();
		bProcessArchiveLoading = A.header->r_bool("header", "auto_load");
	}

	if (bProcessArchiveLoading || strstr(Core.Params, "-auto_load_arch"))
		LoadArchive(A);
	else
		A.close();
}

void CLocatorAPI::unload_archive(CLocatorAPI::archive& A)
{
	files_it	I = m_files.begin();
	for (; I != m_files.end(); ++I)
	{
		const file& entry = *I;
		if (entry.vfs == A.vfs_idx)
		{
#ifndef MASTER_GOLD
			Msg("unregistering file [%s]", I->name);
#endif // #ifndef MASTER_GOLD
			char* str = const_cast<char*>(I->name);
			xr_free(str);
			m_files.erase(I);
			break;
		}
	}
	A.close();
}

bool CLocatorAPI::load_all_unloaded_archives()
{
	bool res = false;
	for (auto& archive : m_archives)
	{
		if (archive.hSrcFile == nullptr)
		{
			LoadArchive(archive);
			res = true;
		}
	}
	return res;
}


void CLocatorAPI::ProcessOne(const char* path, const _finddata_t& entry)
{
	string_path		N;
	xr_strcpy(N, sizeof(N), path);
	xr_strcat(N, entry.name);
	xr_strlwr(N);

	if (entry.attrib&_A_HIDDEN)			return;

	if (entry.attrib&_A_SUBDIR)
	{
		if (bNoRecurse)					return;
		if (!xr_strcmp(entry.name, "."))	return;
		if (!xr_strcmp(entry.name, ".."))	return;
		xr_strcat(N, "\\");
		Register(N, VFS_STANDARD_FILE, 0, 0, entry.size, entry.size, (u32)entry.time_write);
		Recurse(N);
	}
	else {
		if (strext(N) && (!strncmp(strext(N), ".db", 3) || !strncmp(strext(N), ".xdb", 4)))
			ProcessArchive(N);
		else
			Register(N, VFS_STANDARD_FILE, 0, 0, entry.size, entry.size, (u32)entry.time_write);
	}
}

IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{
	return xr_strcmp(x.name, y.name) < 0;
}


bool ignore_name(const char* _name)
{
	// ignore windows hidden thumbs.db
	if (strcmp(_name, "Thumbs.db") == 0)
		return true;

	// ignore processing ".svn" folders
	return (_name[0] == '.' && _name[1] == 's' && _name[2] == 'v' && _name[3] == 'n' && _name[4] == 0);
}

// we need to check for file existance
// because Unicode file names can 
// be interpolated by FindNextFile()

bool ignore_path(const char* _path) {
	HANDLE h = CreateFile(_path, 0, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING, nullptr);

	if (h != INVALID_HANDLE_VALUE)
	{
		CloseHandle(h);
		return false;
	}
	else
		return true;
}

bool CLocatorAPI::Recurse(const char* path)
{
	string_path scanPath;
	xr_strcpy(scanPath, sizeof(scanPath), path);
	xr_strcat(scanPath, "*.*");
	_finddata_t findData;
	intptr_t handle = _findfirst(scanPath, &findData);
	if (handle == -1)
	{
		return false;
	}

	rec_files.reserve(256);
	size_t oldSize = rec_files.size();
	intptr_t done = handle;
	while (done != -1)
	{
		string1024 fullPath;
		bool ignore = false;
		if (m_Flags.test(flNeedCheck))
		{
			xr_strcpy(fullPath, sizeof(fullPath), path);
			xr_strcat(fullPath, findData.name);
			ignore = ignore_name(findData.name) || ignore_path(fullPath);
		}
		else ignore = ignore_name(findData.name);

		if (!ignore)
			rec_files.push_back(findData);
		done = _findnext(handle, &findData);
	}
	_findclose(handle);
	size_t newSize = rec_files.size();
	if (newSize > oldSize)
	{
		std::sort(rec_files.begin() + oldSize, rec_files.end(), pred_str_ff);
		for (size_t i = oldSize; i < newSize; i++)
			ProcessOne(path, rec_files[i]);
		rec_files.erase(rec_files.begin() + oldSize, rec_files.end());
	}
	// insert self
	if (path && path[0] != 0)
		Register(path, VFS_STANDARD_FILE, 0, 0, 0, 0, 0);

	return true;
}

static void searchForFsltx(const char* fs_name, string_path& fsltxPath)
{
	//#TODO: Update code, when std::filesystem is out (not much work, standards don't changing dramatically)
	const char* realFsltxName = nullptr;
	if (fs_name)
	{
		realFsltxName = fs_name;
	}
	else
	{
		realFsltxName = FSLTX;
	}

	//try in working dir
	if (std::experimental::filesystem::exists(realFsltxName))
	{
		xr_strcpy(fsltxPath, realFsltxName);
		return;
	}

	auto tryPathFunc = [realFsltxName](std::experimental::filesystem::path possibleLocationFsltx, string_path& fsltxPath) -> bool
	{
		possibleLocationFsltx.append(realFsltxName);
		if (std::experimental::filesystem::exists(possibleLocationFsltx))
		{
			xr_strcpy(fsltxPath, possibleLocationFsltx.generic_string().c_str());
			return true;
		}
		return false;
	};

	//don't give up, try one directory up
	if (tryPathFunc("../", fsltxPath)) return;

	//don't loose hope. Stand still!
	if (tryPathFunc("../../", fsltxPath)) return;

	//must... find... fs_name... file...
	std::experimental::filesystem::path currentPath = std::experimental::filesystem::current_path();
	std::experimental::filesystem::directory_iterator curPathIter(currentPath);
	for (auto& dirEntry : curPathIter)
	{
		if (std::experimental::filesystem::is_directory(dirEntry))
		{
			if (tryPathFunc(dirEntry, fsltxPath)) return;
		}
	}

	//
}

[[deprecated("Not used anywhere!")]]
void CLocatorAPI::setup_fs_path(const char* fs_name, string_path &fs_path)
{
	xr_strcpy(fs_path, fs_name ? fs_name : "");
	char*				slash = strrchr(fs_path, '\\');
	if (!slash)
		slash = strrchr(fs_path, '/');
	if (!slash) {
		xr_strcpy(fs_path, "");
		return;
	}

	*(slash + 1) = 0;
}

IReader *CLocatorAPI::setup_fs_ltx(const char* fs_name)
{
	string_path	fs_path;
	memset(fs_path, 0, sizeof(fs_path));
	searchForFsltx(fs_name, fs_path);
	CHECK_OR_EXIT(fs_path[0] != 0, make_string("Cannot find fsltx file: \"%s\"\nCheck your working directory", fs_name));
	xr_strlwr(fs_path);
	fsRoot = fs_path;
	fsRoot = std::experimental::filesystem::absolute(fsRoot);
	fsRoot = fsRoot.parent_path();

	Msg("using fs-ltx %s", fs_path);

	HANDLE file_handle;
	size_t file_size;
	IReader *result = nullptr;
	CHECK_OR_EXIT(file_handle_internal(fs_path, file_size, file_handle), make_string("Cannot open file \"%s\".\nCheck your working folder.", fs_name));

	void *buffer = FileDownload(fs_path, file_handle, file_size);
	result = new CTempReader(buffer, (int)file_size, 0);

#ifdef DEBUG
	if (result && m_Flags.is(flBuildCopy | flReady))
		copy_file_to_build(result, fs_path);
#endif // DEBUG

	if (m_Flags.test(flDumpFileActivity))
		_register_open_file(result, fs_path);

	return			(result);
}

void CLocatorAPI::_initialize(u32 flags, const char* target_folder, const char* fs_name)
{
	char _delimiter = '|'; //','
	if (m_Flags.is(flReady))return;
	CTimer t;
	t.Start();
	Log("Initializing File System...");

	m_Flags.set(flags, true);

	// scan root directory
	bNoRecurse = true;
	string4096		buf;

	// append application path
	if (m_Flags.is(flScanAppRoot))
		append_path("$app_root$", Core.ApplicationPath, nullptr, FALSE);


	//-----------------------------------------------------------
	// append application data path
	// target folder 
	if (m_Flags.is(flTargetFolderOnly))
		append_path("$target_folder$", target_folder, nullptr, TRUE);
	else
	{
		IReader			*pFSltx = setup_fs_ltx(fs_name);
		// append all pathes    
		string_path		id, root, add, def, capt;
		const char*			lp_add, *lp_def, *lp_capt;
		string16		b_v;
		string4096		temp;

		while (!pFSltx->eof())
		{
			pFSltx->r_string(buf, sizeof(buf));
			if (buf[0] == ';')		continue;

			_GetItem(buf, 0, id, '=');

			if (!m_Flags.is(flBuildCopy) && (0 == xr_strcmp(id, "$build_copy$")))
				continue;

			_GetItem(buf, 1, temp, '=');
			int cnt = _GetItemCount(temp, _delimiter);
			R_ASSERT2(cnt >= 3, temp);
			u32 fl = 0;
			_GetItem(temp, 0, b_v, _delimiter);

			if (CInifile::IsBOOL(b_v))
				fl |= FS_Path::flRecurse;

			_GetItem(temp, 1, b_v, _delimiter);
			if (CInifile::IsBOOL(b_v))
				fl |= FS_Path::flNotif;

			_GetItem(temp, 2, root, _delimiter);
			_GetItem(temp, 3, add, _delimiter);
			_GetItem(temp, 4, def, _delimiter);
			_GetItem(temp, 5, capt, _delimiter);
			xr_strlwr(id);

			xr_strlwr(root);
			lp_add = (cnt >= 4) ? xr_strlwr(add) : nullptr;
			lp_def = (cnt >= 5) ? def : nullptr;
			lp_capt = (cnt >= 6) ? capt : nullptr;

			auto p_it = pathes.find(root);

			if (p_it == pathes.end() && xr_strcmp(root, "$fs_root$") == 0)
			{
				//Old good fsltx
				//replace root with predefined path
				//xr_strcpy(root, fsRoot.generic_string().c_str());
				FS_Path* P = new FS_Path(xr_strdup(fsRoot.generic_string().c_str()), nullptr, nullptr, nullptr, 0);
				pathes.insert(std::make_pair(xr_strdup("$fs_root$"), P));
				p_it = pathes.find(root);
			}

			FS_Path* P = new FS_Path((p_it != pathes.end()) ? p_it->second->m_Path : root, lp_add, lp_def, lp_capt, fl);
			bNoRecurse = !(fl&FS_Path::flRecurse);
			Recurse(P->m_Path);
			auto I = pathes.insert(std::make_pair(xr_strdup(id), P));
#ifndef DEBUG
			m_Flags.set(flCacheFiles, FALSE);
#endif // DEBUG

			//CHECK_OR_EXIT		(I.second,"The file 'fsgame.ltx' is corrupted (it contains duplicated lines).\nPlease reinstall the game or fix the problem manually.");
		}
		r_close(pFSltx);
		R_ASSERT(path_exist("$app_data_root$"));
	};


	//	u32	M2			= Memory.mem_usage();
	//	Msg				("FS: %d files cached %d archives, %dKb memory used.",m_files.size(),m_archives.size(), (M2-M1)/1024);

	m_Flags.set(flReady, true);

	Msg("Init FileSystem %f sec", t.GetElapsed_sec());
	//-----------------------------------------------------------
	if (strstr(Core.Params, "-overlaypath"))
	{
		string1024				c_newAppPathRoot;
		sscanf(strstr(Core.Params, "-overlaypath ") + 13, "%[^ ] ", c_newAppPathRoot);
		FS_Path* pLogsPath = FS.get_path("$logs$");
		FS_Path* pAppdataPath = FS.get_path("$app_data_root$");


		if (pLogsPath) pLogsPath->_set_root(c_newAppPathRoot);
		if (pAppdataPath)
		{
			pAppdataPath->_set_root(c_newAppPathRoot);
			rescan_path(pAppdataPath->m_Path, pAppdataPath->m_Flags.is(FS_Path::flRecurse));
		}
	}

	rec_files.clear();
	//-----------------------------------------------------------

	if (strstr(Core.Params, "-nolog") == nullptr)
	{
		xrLogger::OpenLogFile();
	}
}

void CLocatorAPI::_destroy()
{
	xrLogger::CloseLog();

	for (const auto & file : m_files)
	{
		char* str = const_cast<char*>(file.name);
		xr_free(str);
	}
	m_files.clear();
	for (auto& it : pathes)
	{
		char* str = const_cast<char*>(it.first);
		xr_free(str);
		xr_delete(it.second);
	}
	pathes.clear();
	for (auto& it : m_archives)
	{
		xr_delete(it.header);
		it.close();
	}
	m_archives.clear();
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* fn)
{
	files_it it = file_find_it(fn);
	return (it != m_files.end()) ? &(*it) : nullptr;
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* path, const char* name)
{
	string_path		temp;
	update_path(temp, path, name);
	return			exist(temp);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, const char* path, const char* name)
{
	update_path(fn, path, name);
	return			exist(fn);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, const char* path, const char* name, const char* ext)
{
	string_path		nm;
	xr_strconcat(nm, name, ext);
	update_path(fn, path, nm);
	return			exist(fn);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* initial, const char* folder, u32 flags)
{
	string_path		N;
	R_ASSERT(initial&&initial[0]);
	update_path(N, initial, folder);
	return			file_list_open(N, flags);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* _path, u32 flags)
{
	R_ASSERT(_path);
	VERIFY(flags);
	// Check if we need rescan paths
	check_pathes();

	string_path		N;

	if (path_exist(_path))
		update_path(N, _path, "");
	else
		xr_strcpy(N, sizeof(N), _path);

	file			desc;
	desc.name = N;
	files_it	I = m_files.find(desc);
	if (I == m_files.end())	return nullptr;

	xr_vector<char*>*	dest = new xr_vector<char*>();

	size_t base_len = xr_strlen(N);
	for (++I; I != m_files.end(); I++)
	{
		const file& entry = *I;
		if (0 != strncmp(entry.name, N, base_len))	break;	// end of list
		const char* end_symbol = entry.name + xr_strlen(entry.name) - 1;
		if ((*end_symbol) != '\\') {
			// file
			if ((flags&FS_ListFiles) == 0)	continue;

			const char* entry_begin = entry.name + base_len;
			if ((flags&FS_RootOnly) && strstr(entry_begin, "\\"))	continue;	// folder in folder
			dest->push_back(xr_strdup(entry_begin));
			char* fname = dest->back();
			if (flags&FS_ClampExt)	if (nullptr != strext(fname)) *strext(fname) = 0;
		}
		else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry.name + base_len;

			if ((flags&FS_RootOnly) && (strstr(entry_begin, "\\") != end_symbol))	continue;	// folder in folder

			dest->push_back(xr_strdup(entry_begin));
		}
	}
	return dest;
}

void	CLocatorAPI::file_list_close(xr_vector<char*>* &lst)
{
	if (lst)
	{
		for (char* I : *lst)
			xr_free(I);
		xr_delete(lst);
	}
}

int CLocatorAPI::file_list(FS_FileSet& dest, const char* path, u32 flags, const char* mask)
{
	R_ASSERT(path);
	VERIFY(flags);
	// Check if we need rescan paths
	check_pathes();

	string_path		N;
	if (path_exist(path))
		update_path(N, path, "");
	else
		xr_strcpy(N, sizeof(N), path);

	file			desc;
	desc.name = N;
	files_it	I = m_files.find(desc);
	if (I == m_files.end())	return 0;

	SStringVec 		masks;
	_SequenceToList(masks, mask);
	BOOL b_mask = !masks.empty();

	size_t base_len = xr_strlen(N);
	for (++I; I != m_files.end(); ++I)
	{
		const file& entry = *I;
		if (0 != strncmp(entry.name, N, base_len))	break;	// end of list
		const char* end_symbol = entry.name + xr_strlen(entry.name) - 1;
		if ((*end_symbol) != '\\')
		{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			const char* entry_begin = entry.name + base_len;
			if ((flags&FS_RootOnly) && strstr(entry_begin, "\\"))	continue;	// folder in folder
			// check extension
			if (b_mask) {
				bool bOK = false;
				for (const auto& it : masks)
				{
					if (PatternMatch(entry_begin, it.c_str()))
					{
						bOK = true;
						break;
					}
				}
				if (!bOK)			continue;
			}
			FS_File file;
			if (flags&FS_ClampExt)
				file.name = EFS.ChangeFileExt(entry_begin, "");
			else
				file.name = entry_begin;


			u32 fl = (entry.vfs != VFS_STANDARD_FILE ? FS_File::flVFS : 0);
			file.size = entry.size_real;
			file.time_write = entry.modif;
			file.attrib = fl;
			dest.insert(std::move(file));
		}
		else {
			// folder
			if ((flags&FS_ListFolders) == 0)	continue;
			const char* entry_begin = entry.name + base_len;

			if ((flags&FS_RootOnly) && (strstr(entry_begin, "\\") != end_symbol))	continue;	// folder in folder
			u32 fl = FS_File::flSubDir | (entry.vfs ? FS_File::flVFS : 0);
			dest.insert(FS_File(entry_begin, entry.size_real, entry.modif, fl));
		}
	}
	return int(dest.size());
}

void CLocatorAPI::check_cached_files(char* fname, const u32 &fname_size, const file &desc, const char* &source_name)
{
	string_path		fname_copy;
	if (pathes.size() <= 1)
		return;

	if (!path_exist("$server_root$"))
		return;

	const char*			path_base = get_path("$server_root$")->m_Path;
	u32				len_base = xr_strlen(path_base);
	const char*			path_file = fname;
	u32				len_file = xr_strlen(path_file);
	if (len_file <= len_base)
		return;

	if ((len_base == 1) && (*path_base == '\\'))
		len_base = 0;

	if (!memcmp(path_base, fname, len_base))
		return;

	bool bCopy = false;

	string_path	fname_in_cache;
	update_path(fname_in_cache, "$cache$", path_file + len_base);
	files_it	fit = file_find_it(fname_in_cache);
	if (fit != m_files.end())
	{
		// use
		const file&	fc = *fit;
		if ((fc.size_real == desc.size_real) && (fc.modif == desc.modif)) {
			// use
		}
		else {
			// copy & use
			Msg("copy: db[%X],cache[%X] - '%s', ", desc.modif, fc.modif, fname);
			bCopy = true;
		}
	}
	else
		// copy & use
		bCopy = true;

	// copy if need
	if (bCopy) {
		IReader		*_src;
		if (desc.size_real < 256 * 1024)
			_src = new CFileReader(fname);
		else
			_src = new CVirtualFileReader(fname);
		IWriter* _dst = new CFileWriter(fname_in_cache, false);

		_dst->w(_src->pointer(), _src->length());
		xr_delete(_dst);
		xr_delete(_src);
		set_file_age(fname_in_cache, desc.modif);
		Register(fname_in_cache, VFS_STANDARD_FILE, 0, 0, desc.size_real, desc.size_real, desc.modif);
	}

	// Use
	source_name = &fname_copy[0];
	xr_strcpy(fname_copy, sizeof(fname_copy), fname);
	xr_strcpy(fname, fname_size, fname_in_cache);
}

void CLocatorAPI::file_from_cache_impl(IReader *&R, char* fname, const file &desc)
{
	if (desc.size_real < 16 * 1024)
		R = new CFileReader(fname);
	else
		R = new CVirtualFileReader(fname);
}

void CLocatorAPI::file_from_cache_impl(CStreamReader *&R, char* fname, const file &desc)
{
	CFileStreamReader			*r = new CFileStreamReader();
	r->construct(fname, BIG_FILE_READER_WINDOW_SIZE);
	R = r;
}

template <typename T>
void CLocatorAPI::file_from_cache(T *&R, char* fname, const u32 &fname_size, const file &desc, const char* &source_name)
{
#ifdef DEBUG
	if (m_Flags.is(flCacheFiles))
		check_cached_files(fname, fname_size, desc, source_name);
#endif // DEBUG

	file_from_cache_impl(R, fname, desc);
}

void CLocatorAPI::file_from_archive(IReader *&R, const char* fname, const file &desc)
{
	// Archived one
	archive& A = m_archives[desc.vfs];
	u32 start = (desc.ptr / dwAllocGranularity)*dwAllocGranularity;
	u32 end = (desc.ptr + desc.size_compressed) / dwAllocGranularity;
	if ((desc.ptr + desc.size_compressed) % dwAllocGranularity)	end += 1;
	end *= dwAllocGranularity;
	if (end > A.size)				end = A.size;
	u32 sz = (end - start);
	u8* ptr = (u8*)MapViewOfFile(A.hSrcMap, FILE_MAP_READ, 0, start, sz); VERIFY3(ptr, "cannot create file mapping on file", fname);

	string512					temp;
	xr_sprintf(temp, sizeof(temp), "%s:%s", *A.path, fname);

	u32 ptr_offs = desc.ptr - start;

	if (desc.size_real == desc.size_compressed)
	{
		R = new CPackReader(ptr, ptr + ptr_offs, desc.size_real);
		return;
	}

	// Compressed
	u8* dest = xr_alloc<u8>(desc.size_real);
	XRay::Compress::RT::RtcDecompress(dest, desc.size_real, ptr + ptr_offs, desc.size_compressed);
	R = new CTempReader(dest, desc.size_real, 0);
	UnmapViewOfFile(ptr);
}

void CLocatorAPI::file_from_archive(CStreamReader *&R, const char* fname, const file &desc)
{
	archive	&A = m_archives[desc.vfs];
	R_ASSERT3(desc.size_compressed == desc.size_real,
		"cannot use stream reading for compressed data, do not compress data to be streamed", fname);

	R = new CStreamReader();
	R->construct(A.hSrcMap, desc.ptr, desc.size_compressed, A.size, BIG_FILE_READER_WINDOW_SIZE);
}

void CLocatorAPI::copy_file_to_build(IWriter *W, IReader *r)
{
	W->w(r->pointer(), r->length());
}

void CLocatorAPI::copy_file_to_build(IWriter *W, CStreamReader *r)
{
	size_t				buffer_size = r->length();
	u8					*buffer = xr_alloc<u8>(buffer_size);
	r->r(buffer, buffer_size);
	W->w(buffer, buffer_size);
	xr_free(buffer);
	r->seek(0);
}

template <typename T>
void CLocatorAPI::copy_file_to_build(T *&r, const char* source_name)
{
	string_path	cpy_name;
	string_path	e_cpy_name;
	FS_Path* 	P;

	string_path				fs_root;
	update_path(fs_root, "$fs_root$", "");
	const char* const position = strstr(source_name, fs_root);
	if (position == source_name)
		update_path(cpy_name, "$build_copy$", source_name + xr_strlen(fs_root));
	else
		update_path(cpy_name, "$build_copy$", source_name);

	IWriter* W = w_open(cpy_name);
	if (!W) {
		Msg("!Can't build: %s", source_name);
		return;
	}

	copy_file_to_build(W, r);
	w_close(W);
	set_file_age(cpy_name, get_file_age(source_name));
	if (!m_Flags.is(flEBuildCopy))
		return;

	const char* ext = strext(cpy_name);
	if (!ext)
		return;

	IReader* R = nullptr;
	if (0 == xr_strcmp(ext, ".dds")) {
		P = get_path("$game_textures$");
		update_path(e_cpy_name, "$textures$", source_name + xr_strlen(P->m_Path));
		// tga
		*strext(e_cpy_name) = 0;
		xr_strcat(e_cpy_name, ".tga");
		r_close(R = r_open(e_cpy_name));
		// thm
		*strext(e_cpy_name) = 0;
		xr_strcat(e_cpy_name, ".thm");
		r_close(R = r_open(e_cpy_name));
		return;
	}

	if (0 == xr_strcmp(ext, ".ogg")) {
		P = get_path("$game_sounds$");
		update_path(e_cpy_name, "$sounds$", source_name + xr_strlen(P->m_Path));
		// wav
		*strext(e_cpy_name) = 0;
		xr_strcat(e_cpy_name, ".wav");
		r_close(R = r_open(e_cpy_name));
		// thm
		*strext(e_cpy_name) = 0;
		xr_strcat(e_cpy_name, ".thm");
		r_close(R = r_open(e_cpy_name));
		return;
	}

	if (0 == xr_strcmp(ext, ".object")) {
		xr_strcpy(e_cpy_name, sizeof(e_cpy_name), source_name);
		// object thm
		*strext(e_cpy_name) = 0;
		xr_strcat(e_cpy_name, ".thm");
		R = r_open(e_cpy_name);
		if (R)		r_close(R);
	}
}

bool CLocatorAPI::check_for_file(const char* path, const char* _fname, string_path& fname, const file *&desc)
{
	check_pathes();

	// correct path
	xr_strcpy(fname, _fname);
	xr_strlwr(fname);
	if (path&&path[0])
		update_path(fname, path, fname);

	// Search entry
	file					desc_f;
	desc_f.name = fname;

	files_it				I = m_files.find(desc_f);
	if (I == m_files.end())
		return				(false);

	++dwOpenCounter;
	desc = &*I;
	return					(true);
}

template <typename T>
T *CLocatorAPI::r_open_impl(const char* path, const char* _fname)
{
	T						*R = 0;
	string_path				fname;
	const file				*desc = nullptr;
	const char*					source_name = &fname[0];

	if (!check_for_file(path, _fname, fname, desc))
		return				(0);

	// OK, analyse
	if (VFS_STANDARD_FILE == desc->vfs)
		file_from_cache(R, fname, sizeof(fname), *desc, source_name);
	else
		file_from_archive(R, fname, *desc);

#ifdef DEBUG
	if (R && m_Flags.is(flBuildCopy | flReady))
		copy_file_to_build(R, source_name);
#endif // DEBUG

	if (m_Flags.test(flDumpFileActivity))
		_register_open_file(R, fname);

	return					(R);
}

CStreamReader* CLocatorAPI::rs_open(const char* path, const char* _fname)
{
	return (r_open_impl<CStreamReader>(path, _fname));
}

IReader *CLocatorAPI::r_open(const char* path, const char* _fname)
{
	return (r_open_impl<IReader>(path, _fname));
}

void	CLocatorAPI::r_close(IReader* &fs)
{
	if (m_Flags.test(flDumpFileActivity))
		_unregister_open_file(fs);

	xr_delete(fs);
}

void CLocatorAPI::r_close(CStreamReader* &fs)
{
	if (m_Flags.test(flDumpFileActivity))
		_unregister_open_file(fs);

	fs->close();
}

IWriter* CLocatorAPI::w_open(const char* path, const char* _fname)
{
	string_path	fname;
	xr_strcpy(fname, _fname);
	xr_strlwr(fname);
	if (path&&path[0]) update_path(fname, path, fname);
	return new CFileWriter(fname, false);
}

IWriter* CLocatorAPI::w_open_ex(const char* path, const char* _fname)
{
	string_path	fname;
	xr_strcpy(fname, _fname);
	xr_strlwr(fname);//,".$");
	if (path&&path[0]) update_path(fname, path, fname);
	return new CFileWriter(fname, true);
}

void	CLocatorAPI::w_close(IWriter* &S)
{
	if (S)
	{
		R_ASSERT(S->fName.size());
		string_path	fname;
		xr_strcpy(fname, sizeof(fname), *S->fName);
		bool bReg = S->valid();
		xr_delete(S);

		if (bReg)
		{
			struct _stat st;
			_stat(fname, &st);
			Register(fname, VFS_STANDARD_FILE, 0, 0, st.st_size, st.st_size, (u32)st.st_mtime);
		}
	}
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(const char* fname)
{
	check_pathes();

	file			desc_f;
	string_path		file_name;
	VERIFY(xr_strlen(fname) * sizeof(char) < sizeof(file_name));
	xr_strcpy(file_name, sizeof(file_name), fname);
	desc_f.name = file_name;
	files_it I = m_files.find(desc_f);
	return			(I);
}

BOOL CLocatorAPI::dir_delete(const char* path, const char* nm, BOOL remove_files)
{
	string_path	fpath;
	if (path&&path[0]) 	update_path(fpath, path, nm);
	else				xr_strcpy(fpath, sizeof(fpath), nm);

	files_set 	folders;
	files_it I;
	// remove files
	I = file_find_it(fpath);
	if (I != m_files.end())
	{
		size_t base_len = xr_strlen(fpath);
		for (; I != m_files.end(); )
		{
			files_it cur_item = I;
			const file& entry = *cur_item;
			I = cur_item; I++;
			if (0 != strncmp(entry.name, fpath, base_len))
				break;	// end of list
			const char* end_symbol = entry.name + xr_strlen(entry.name) - 1;

			if ((*end_symbol) != '\\')
			{
				if (!remove_files)
					return FALSE;
				unlink(entry.name);
				m_files.erase(cur_item);
			}
			else
			{
				folders.insert(entry);
			}
		}
	}
	// remove folders
	files_set::reverse_iterator r_it = folders.rbegin();
	for (; r_it != folders.rend(); r_it++)
	{
		const char* end_symbol = r_it->name + xr_strlen(r_it->name) - 1;
		if ((*end_symbol) == '\\')
		{
			_rmdir(r_it->name);
			m_files.erase(*r_it);
		}
	}
	return TRUE;
}

void CLocatorAPI::file_delete(const char* path, const char* nm)
{
	string_path	fname;
	if (path&&path[0])
		update_path(fname, path, nm);
	else
		xr_strcpy(fname, sizeof(fname), nm);

	const files_it I = file_find_it(fname);

	if (I != m_files.end())
	{
		// remove file
		unlink(I->name);
		char* str = const_cast<char*>(I->name);
		xr_free(str);
		m_files.erase(I);
	}
}

void CLocatorAPI::file_copy(const char* src, const char* dest)
{
	if (exist(src))
	{
		IReader* S = r_open(src);
		if (S)
		{
			IWriter* D = w_open(dest);
			if (D)
			{
				D->w(S->pointer(), S->length());
				w_close(D);
			}
			r_close(S);
		}
	}
}

void CLocatorAPI::file_rename(const char* src, const char* dest, bool bOwerwrite)
{
	files_it S = file_find_it(src);

	if (S != m_files.end())
	{
		files_it D = file_find_it(dest);
		if (D != m_files.end())
		{
			if (!bOwerwrite)
				return;

			_unlink(D->name);
			char* str = const_cast<char*>(D->name);
			xr_free(str);
			m_files.erase(D);
		}

		file new_desc = *S;
		// remove existing item
		char* str = const_cast<char*>(S->name);
		xr_free(str);
		m_files.erase(S);
		// insert updated item
		new_desc.name = xr_strlwr(xr_strdup(dest));
		m_files.insert(new_desc);

		// physically rename file
		createPath(dest);
		rename(src, dest);
	}
}

int	CLocatorAPI::file_length(const char* src)
{
	files_it	I = file_find_it(src);
	return (I != m_files.end()) ? I->size_real : -1;
}

bool CLocatorAPI::path_exist(const char* path)
{
	return pathes.find(path) != pathes.end();
}

FS_Path* CLocatorAPI::append_path(const char* path_alias, const char* root, const char* add, BOOL recursive)
{
	VERIFY(root);
	VERIFY(!path_exist(path_alias));
	FS_Path* P = new FS_Path(root, add, nullptr, nullptr, 0);
	bNoRecurse = !recursive;
	Recurse(P->m_Path);
	pathes.insert(std::make_pair(xr_strdup(path_alias), P));
	return P;
}

FS_Path* CLocatorAPI::get_path(const char* path)
{
	if (strstr(path, "$"))
	{
		auto P = pathes.find(path);
		R_ASSERT3(P != pathes.end(), path, "Please update fsgame.ltx");
		return P->second;
	}
	else
	{
		FS_Path* pTempFSPath = new FS_Path(path, "", nullptr, nullptr, 0);
		return pTempFSPath;
	}
}

const char* CLocatorAPI::update_path(string_path& dest, const char* initial, const char* src)
{
	return get_path(initial)->_update(dest, src);
}


u32 CLocatorAPI::get_file_age(const char* nm)
{
	check_pathes();

	files_it I = file_find_it(nm);
	return (I != m_files.end()) ? I->modif : u32(-1);
}

void CLocatorAPI::set_file_age(const char* nm, u32 age)
{
	check_pathes();

	// set file
	_utimbuf	tm;
	tm.actime = age;
	tm.modtime = age;
	int res = _utime(nm, &tm);
	if (0 != res)
		Msg("!Can't set file age: '%s'. Error: '%s'", nm, _sys_errlist[errno]);
	else
	{
		// update record
		files_it I = file_find_it(nm);
		if (I != m_files.end())
		{
			file& F = (file&)*I;
			F.modif = age;
		}
	}
}

void CLocatorAPI::rescan_path(const char* full_path, BOOL bRecurse)
{
	file desc;
	desc.name = full_path;
	files_it	I = m_files.lower_bound(desc);
	if (I == m_files.end())	return;

	size_t base_len = xr_strlen(full_path);
	for (; I != m_files.end(); )
	{
		files_it cur_item = I;
		const file& entry = *cur_item;
		I = cur_item; I++;
		if (0 != strncmp(entry.name, full_path, base_len))
			break;	// end of list
		if (entry.vfs != VFS_STANDARD_FILE)
			continue;
		const char* entry_begin = entry.name + base_len;
		if (!bRecurse && strstr(entry_begin, "\\"))
			continue;
		// erase item
		char* str = const_cast<char*>(cur_item->name);
		xr_free(str);
		m_files.erase(cur_item);
	}
	bNoRecurse = !bRecurse;
	Recurse(full_path);
}

void  CLocatorAPI::rescan_pathes()
{
	m_Flags.set(flNeedRescan, false);
	for (const auto& it : pathes)
	{
		FS_Path* P = it.second;
		if (P->m_Flags.is(FS_Path::flNeedRescan)) {
			rescan_path(P->m_Path, P->m_Flags.is(FS_Path::flRecurse));
			P->m_Flags.set(FS_Path::flNeedRescan, false);
		}
	}
}

void CLocatorAPI::lock_rescan()
{
	m_iLockRescan++;
}

void CLocatorAPI::unlock_rescan()
{
	m_iLockRescan--;  VERIFY(m_iLockRescan >= 0);
	if ((!m_iLockRescan) && m_Flags.is(flNeedRescan))
		rescan_pathes();
}

bool CLocatorAPI::getFileName(LPCSTR path, string512& outFilename)
{
	std::experimental::filesystem::path stdPath = path;
	if (stdPath.has_filename())
	{
		std::experimental::filesystem::path fileNamePath = stdPath.filename();
		std::string fileNameStr = fileNamePath.u8string();
		R_ASSERT2(fileNameStr.size() < sizeof(string512), fileNameStr.c_str());

		xr_strcpy(outFilename, fileNameStr.c_str());
		return true;
	}

	return false;
}

void CLocatorAPI::check_pathes()
{
	if (m_Flags.is(flNeedRescan) && (!m_iLockRescan))
	{
		lock_rescan();
		rescan_pathes();
		unlock_rescan();
	}
}

BOOL CLocatorAPI::can_write_to_folder(const char* path)
{
	if (path && path[0])
	{
		string_path temp;
		const char* fn = "$!#%TEMP%#!$.$$$";
		xr_strconcat(temp, path, path[xr_strlen(path) - 1] != '\\' ? "\\" : "", fn);
		FILE* hf = fopen(temp, "wb");
		if (!hf)
			return FALSE;
		else
		{
			fclose(hf);
			unlink(temp);
			return TRUE;
		}
	}
	else
		return FALSE;
}

BOOL CLocatorAPI::can_write_to_alias(const char* path)
{
	string_path temp;
	update_path(temp, path, "");
	return can_write_to_folder(temp);
}

BOOL CLocatorAPI::can_modify_file(const char* fname)
{
	FILE* hf = fopen(fname, "r+b");
	if (hf)
	{
		fclose(hf);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CLocatorAPI::can_modify_file(const char* path, const char* name)
{
	string_path temp;
	update_path(temp, path, name);
	return can_modify_file(temp);
}