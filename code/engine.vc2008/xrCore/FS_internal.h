#pragma once

#include "lzhuf.h"
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>

#ifndef SPECTRE
#include <filesystem>
#endif


void* FileDownload(const char* fn, size_t* pdwSize = nullptr);
void* FileDownload(const char* file_name, HANDLE file_handle, size_t file_size);
void FileCompress(const char* fn, const char* sign, void* data, const size_t size);
void* FileDecompress(const char* fn, const char* sign, size_t* size = nullptr);

class CFileWriter : public IWriter 
{
    HANDLE hf = INVALID_HANDLE_VALUE;
	u64 pos = 0;
public:
    CFileWriter(const char* name, const bool exclusive) 
	{
        R_ASSERT(name && name[0]);
        fName = name;

		DWORD shareMode = exclusive ? 0 : FILE_SHARE_READ;
		hf = CreateFileA(*fName, GENERIC_ALL, shareMode, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!valid())
		{
			// try create path
			createPath(*fName, true, true);
			hf = CreateFileA(*fName, GENERIC_ALL, shareMode, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!valid())
			{
				Msg("!Can't write file: '%s'. Error: '%s'.", *fName, Debug.error2string(GetLastError()));
			}
		}
    }

    virtual ~CFileWriter() 
	{
		CloseHandle(hf);
    }
    // kernel
    void w(const void* _ptr, const size_t count) override 
	{
        if (valid() && (0 != count)) 
		{
			u8* cursor = (u8*)_ptr;

			DWORD safeCount = DWORD(count);
			DWORD totalBytesWritten = 0;

			while (totalBytesWritten < count)
			{
				DWORD bytesWritten = 0;
				WriteFile(hf, cursor + totalBytesWritten, safeCount - totalBytesWritten, &bytesWritten, NULL);
				totalBytesWritten += bytesWritten;
			}

			pos += totalBytesWritten;
        }
    }

    void seek(const size_t InPos) override 
	{
		if (valid())
		{
			LARGE_INTEGER newPos; newPos.QuadPart = InPos;
			SetFilePointerEx(hf, newPos, (PLARGE_INTEGER)&pos, FILE_BEGIN);
		}
    }

    IC size_t tell() override { return size_t(pos); }
    IC bool valid() override { return hf != INVALID_HANDLE_VALUE; }

    IC void flush() override 
	{
		if (valid())
		{
			FlushFileBuffers(hf);
		}
    }
};

// It automatically frees memory after destruction
class CTempReader : public IReader 
{
public:
    CTempReader(void* data, const int size, const int iterpos) : IReader(data, size, iterpos) {}
    virtual ~CTempReader();
};

class CPackReader : public IReader {
    void* base_address;
public:
    CPackReader(void* base, void* data, const int size) : IReader(data, size) 
	{
        base_address = base;
    }
    virtual ~CPackReader();
};

class XRCORE_API CFileReader : public IReader 
{
public:
    CFileReader(const char* name);
    virtual ~CFileReader();
};

class CCompressedReader : public IReader 
{
public:
    CCompressedReader(const char* name, const char* sign);
    virtual ~CCompressedReader();
};

class CVirtualFileReader : public IReader 
{
    void *hSrcFile, *hSrcMap;
public:
    CVirtualFileReader(const char* cFileName);
    virtual ~CVirtualFileReader();
};