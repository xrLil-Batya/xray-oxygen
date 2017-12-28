// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

#include "FS_internal.h"

XRCORE_API CLocatorAPI* xr_FS	= nullptr;

#ifdef _EDITOR
#define FSLTX	"fs.ltx"
#else
#define FSLTX	"fsgame.ltx"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLocatorAPI::CLocatorAPI()
{
    m_Flags.zero		();
	// get page size
	SYSTEM_INFO			sys_inf;
	GetSystemInfo		(&sys_inf);
	dwAllocGranularity	= sys_inf.dwAllocationGranularity;
	dwOpenCounter		= 0;
}

CLocatorAPI::~CLocatorAPI()
{
}

void CLocatorAPI::_initialize	(u32 flags, LPCSTR target_folder, LPCSTR fs_fname)
{
	char _delimiter = '|'; //','
	if (m_Flags.is(flReady))return;

	Log				("Initializing File System...");
	m_Flags.set		(flags,TRUE);

	append_path	("$fs_root$", "", 0, FALSE);

	// append application path

	if (m_Flags.is(flScanAppRoot)){
		append_path		("$app_root$",Core.ApplicationPath,0,FALSE);
    }
	if (m_Flags.is(flTargetFolderOnly)){
		append_path		("$target_folder$",target_folder,0,TRUE);
	}else{
		IReader* F		= r_open((fs_fname&&fs_fname[0])?fs_fname:FSLTX); 
		if (!F&&m_Flags.is(flScanAppRoot))
			F			= r_open("$app_root$",(fs_fname&&fs_fname[0])?fs_fname:FSLTX); 
		R_ASSERT3		(F,"Can't open file:", (fs_fname&&fs_fname[0])?fs_fname:FSLTX);
		// append all pathes    
		string_path		buf;
		string_path		id, temp, root, add, def, capt;
		LPCSTR			lp_add, lp_def, lp_capt;
		string16		b_v;
		while(!F->eof()){
			F->r_string	(buf,sizeof(buf));
			_GetItem(buf,0,id,'=');
			if (id[0]==';') continue;
			_GetItem(buf,1,temp,'=');
			int cnt		= _GetItemCount(temp,_delimiter);  R_ASSERT(cnt>=3);
			u32 fl		= 0;
			_GetItem	(temp,0,b_v,_delimiter);	if (CInifile::IsBOOL(b_v)) fl |= FS_Path::flRecurse;
			_GetItem	(temp,1,b_v,_delimiter);	if (CInifile::IsBOOL(b_v)) fl |= FS_Path::flNotif;
			_GetItem	(temp,2,root,_delimiter);
			_GetItem	(temp,3,add,_delimiter);
			_GetItem	(temp,4,def,_delimiter);
			_GetItem	(temp,5,capt,_delimiter);
			xr_strlwr	(id);			if (!m_Flags.is(flBuildCopy)&&(0==xr_strcmp(id,"$build_copy$"))) continue;
			xr_strlwr	(root);
			lp_add		=(cnt>=4)?xr_strlwr(add):0;
			lp_def		=(cnt>=5)?def:0;
			lp_capt		=(cnt>=6)?capt:0;
			auto p_it = pathes.find(root);

			FS_Path* P	= xr_new<FS_Path>((p_it!=pathes.end())?p_it->second->m_Path:root,lp_add,lp_def,lp_capt,fl);
			auto I = pathes.insert(std::make_pair(xr_strdup(id),P));
			
			R_ASSERT	(I.second);
		}
		r_close			(F);
	};

	m_Flags.set		(flReady,TRUE);

	CreateLog		(0!=strstr(Core.Params,"-nolog"));
}

void CLocatorAPI::_destroy		()
{
	CloseLog		();

	for(auto &p_it: pathes)
	{
		char* str	= LPSTR(p_it.first);
		xr_free		(str);
		xr_delete	(p_it.second);
    }
	pathes.clear	();
}

bool ignore_name(const char* _name)
{
	// ignore windows hidden thumbs.db
	if (strcmp(_name, "Thumbs.db") == 0)
		return true;
	// ignore processing ".svn" folders
	return (_name[0] == '.' && _name[1] == 's' && _name[2] == 'v' && _name[3] == 'n' && _name[4] == 0);
}

typedef void	(__stdcall * TOnFind)	(_finddata_t&, void*);


struct file_list_cb_data
{
	size_t 		base_len;
    u32 		flags;
    SStringVec* masks;
    FS_FileSet* dest;
};

void __stdcall file_list_cb(_finddata_t& entry, void* data)
{
	file_list_cb_data*	D		= (file_list_cb_data*)data;

    LPCSTR end_symbol 			= entry.name+xr_strlen(entry.name)-1;
    if ((*end_symbol)!='\\'){
        // file
        if ((D->flags&FS_ListFiles) == 0)	return;
        LPCSTR entry_begin 		= entry.name+D->base_len;
        if ((D->flags&FS_RootOnly)&&strstr(entry_begin,"\\"))	return;	// folder in folder
        // check extension
        if (D->masks){
            bool bOK			= false;
            for (xr_string &it: *D->masks)
                if (PatternMatch(entry_begin,it.c_str())){bOK=true; break;}
            if (!bOK)			return;
        }
        xr_string fn			= entry_begin;
        // insert file entry
        if (D->flags&FS_ClampExt) fn=EFS.ChangeFileExt(fn,"");
        D->dest->insert			(FS_File(fn,entry));
    } else {
        // folder
        if ((D->flags&FS_ListFolders) == 0)	return;
        LPCSTR entry_begin 		= entry.name+D->base_len;
        D->dest->insert			(FS_File(entry_begin,entry));
    }
}

int CLocatorAPI::file_list(FS_FileSet& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
	R_ASSERT(path);
	VERIFY(flags);

	string_path		fpath;
	if (path_exist(path))
		update_path(fpath, path, "");
	else
		strcpy(fpath, path);

	// build mask
	SStringVec 		masks;
	_SequenceToList(masks, mask);

	file_list_cb_data data;
	data.base_len = xr_strlen(fpath);
	data.flags = flags;
	data.masks = masks.empty() ? 0 : &masks;
	data.dest = &dest;

	Recurse(fpath);
	return dest.size();
}
struct _open_file
{
	union {
		IReader*		_reader;
		CStreamReader*	_stream_reader;
	};
	shared_str			_fn;
	u32					_used;
};

struct eq_fname_check {
	shared_str _val;
	eq_fname_check(shared_str s) { _val = s; }
	bool operator () (_open_file& itm) {
		return (_val == itm._fn && itm._reader != NULL);
	}
};

struct eq_fname_free {
	shared_str _val;
	eq_fname_free(shared_str s) { _val = s; }
	bool operator () (_open_file& itm) {
		return (_val == itm._fn && itm._reader == NULL);
	}
};

XRCORE_API xr_vector<_open_file>	g_open_files;

void _check_open_file(const shared_str& _fname)
{
	xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_check(_fname));
	if (it != g_open_files.end())
		Log("file opened at least twice", _fname.c_str());
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

IReader* CLocatorAPI::r_open	(LPCSTR path, LPCSTR _fname)
{
	IReader* R = 0;

	// correct path
	string_path fname;
	strcpy(fname,_fname);
	xr_strlwr(fname);
	if (path&&path[0]) update_path(fname,path,fname);

	// Search entry
	const file *desc = 0;
	const char* source_name = &fname[0];

	if (!check_for_file(path, _fname, fname, desc))
		return				(0);

	// OK, analyse
	if (0xffffffff == desc->vfs)
		file_from_cache(R, fname, sizeof(fname), *desc, source_name);
	else
		file_from_archive(R, fname, *desc);

#ifdef DEBUG
	if (R && m_Flags.is(flBuildCopy | flReady))
		copy_file_to_build(R, source_name);
#endif // DEBUG

	if (m_Flags.test(flDumpFileActivity))
		_register_open_file(R, fname);
	return R;
}

void	CLocatorAPI::r_close	(IReader* &fs)
{
	xr_delete	(fs);
}

IWriter* CLocatorAPI::w_open	(LPCSTR path, LPCSTR _fname)
{
	string_path	fname;
	xr_strlwr(strcpy(fname,_fname));//,".$");
	if (path&&path[0]) update_path(fname,path,fname);
    CFileWriter* W 	= xr_new<CFileWriter>(fname,false); 
#ifdef _EDITOR
	if (!W->valid()) xr_delete(W);
#endif    
	return W;
}

IWriter* CLocatorAPI::w_open_ex	(LPCSTR path, LPCSTR _fname)
{
	string_path	fname;
	xr_strlwr(strcpy(fname,_fname));//,".$");
	if (path&&path[0]) update_path(fname,path,fname);
    CFileWriter* W 	= xr_new<CFileWriter>(fname,true); 
#ifdef _EDITOR
	if (!W->valid()) xr_delete(W);
#endif    
	return W;
}

void	CLocatorAPI::w_close(IWriter* &S)
{
	if (S){
        R_ASSERT	(S->fName.size());
        xr_delete	(S);
    }
}

struct dir_delete_cb_data
{
	FS_FileSet*		folders;
    BOOL			remove_files;
};

void __stdcall dir_delete_cb(_finddata_t& entry, void* data)
{
	dir_delete_cb_data*	D		= (dir_delete_cb_data*)data;

    if (entry.attrib&_A_SUBDIR)	D->folders->insert	(FS_File(entry));
    else if (D->remove_files)	FS.file_delete		(entry.name);
}

BOOL CLocatorAPI::dir_delete(LPCSTR initial, LPCSTR nm, BOOL remove_files)
{
	string_path			fpath;
	if (initial&&initial[0])
    	update_path	(fpath,initial,nm);
    else
    	strcpy		(fpath,nm);

    FS_FileSet 			folders;
    folders.insert		(FS_File(fpath));

    // recurse find
    dir_delete_cb_data	data;
	data.folders		= &folders;
    data.remove_files	= remove_files;
    Recurse				(fpath);

    // remove folders
    FS_FileSet::reverse_iterator r_it = folders.rbegin();
    for (;r_it!=folders.rend();r_it++)
		_rmdir			(r_it->name.c_str());
    return TRUE;
}                                                

void CLocatorAPI::file_delete(LPCSTR path, LPCSTR nm)
{
	string_path	fname;
	if (path&&path[0])
    	update_path	(fname,path,nm);
    else
    	strcpy		(fname,nm);
    unlink			(fname);
}

void CLocatorAPI::file_copy(LPCSTR src, LPCSTR dest)
{
	if (exist(src)){
        IReader* S		= r_open(src);
        if (S){
            IWriter* D	= w_open(dest);
            if (D){
                D->w	(S->pointer(),S->length());
                w_close	(D);
            }
            r_close		(S);
        }
	}
}

void CLocatorAPI::file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite)
{
	if (bOwerwrite&&exist(dest)) unlink(dest);
    // physically rename file
	createPath(dest);
    rename(src,dest);
}

int	CLocatorAPI::file_length(LPCSTR src)
{
	files_it	I = file_find_it(src);
	return (I != m_files.end()) ? I->size_real : -1;
}

FS_Path* CLocatorAPI::append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive)
{
	VERIFY			(root/*&&root[0]*/);
	VERIFY			(false==path_exist(path_alias));
	FS_Path* P		= xr_new<FS_Path>(root,add,LPCSTR(0),LPCSTR(0),0);
	pathes.insert	(std::make_pair(xr_strdup(path_alias),P));
	return P;
}

FS_Path* CLocatorAPI::get_path(LPCSTR path)
{
    auto P = pathes.find(path); 
    R_ASSERT2(P!=pathes.end(),path);
    return P->second;
}

LPCSTR CLocatorAPI::update_path(string_path& dest, LPCSTR initial, LPCSTR src)
{
    return get_path(initial)->_update(dest,src);
}

BOOL CLocatorAPI::can_write_to_folder(LPCSTR path)
{
	if (path&&path[0]){
		string_path		temp;       
        LPCSTR fn		= "$!#%TEMP%#!$.$$$";
	    strconcat		(sizeof(temp),temp,path,path[xr_strlen(path)-1]!='\\'?"\\":"",fn);
		FILE* hf		= fopen	(temp, "wb");
		if (hf==0)		return FALSE;
        else{
        	fclose 		(hf);
	    	unlink		(temp);
            return 		TRUE;
        }
    }else{
    	return 			FALSE;
    }
}

BOOL CLocatorAPI::can_write_to_alias(LPCSTR path)
{
	string_path			temp;       
    update_path			(temp,path,"");
	return can_write_to_folder(temp);
}

BOOL CLocatorAPI::can_modify_file(LPCSTR fname)
{
	FILE* hf			= fopen	(fname, "r+b");
    if (hf){	
    	fclose			(hf);
        return 			TRUE;
    }else{
    	return 			FALSE;
    }
}

BOOL CLocatorAPI::can_modify_file(LPCSTR path, LPCSTR name)
{
	string_path			temp;       
    update_path			(temp,path,name);
	return can_modify_file(temp);
}
///////////////////////////////////////////////////////
///// Import (LocatorAPI.cpp)

void CLocatorAPI::file_from_archive(IReader *&R, const char* fname, const file &desc)
{
	// Archived one
	archive& A = m_archives[desc.vfs];
	u32 start = (desc.ptr / dwAllocGranularity)*dwAllocGranularity;
	u32 end = (desc.ptr + desc.size_compressed) / dwAllocGranularity;
	if ((desc.ptr + desc.size_compressed) % dwAllocGranularity)	end += 1;
	end *= dwAllocGranularity;
	if (end>A.size)				end = A.size;
	u32 sz = (end - start);
	u8* ptr = (u8*)MapViewOfFile(A.hSrcMap, FILE_MAP_READ, 0, start, sz); VERIFY3(ptr, "cannot create file mapping on file", fname);

	string512					temp;
	xr_sprintf(temp, sizeof(temp), "%s:%s", *A.path, fname);

#ifdef FS_DEBUG
	register_file_mapping(ptr, sz, temp);
#endif // DEBUG

	u32 ptr_offs = desc.ptr - start;
	if (desc.size_real == desc.size_compressed) {
		R = new CPackReader(ptr, ptr + ptr_offs, desc.size_real);
		return;
	}

	// Compressed
	u8*							dest = xr_alloc<u8>(desc.size_real);
	rtc_decompress(dest, desc.size_real, ptr + ptr_offs, desc.size_compressed);
	R = new CTempReader(dest, desc.size_real, 0);
	UnmapViewOfFile(ptr);

#ifdef FS_DEBUG
	unregister_file_mapping(ptr, sz);
#endif // DEBUG
}


void CLocatorAPI::file_from_cache_impl(IReader *&R, char* fname, const file &desc)
{
	if (desc.size_real < 16 * 1024)
		R = new CFileReader(fname);
	else
		R = new CVirtualFileReader(fname);
}

#include "file_stream_reader.h"

void CLocatorAPI::file_from_cache_impl(CStreamReader *&R, char* fname, const file &desc)
{
	CFileStreamReader			*r = new CFileStreamReader();
	r->construct(fname, 1024 * 1024);
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

bool CLocatorAPI::check_for_file(const char* path, const char* _fname, string_path& fname, const file *&desc)
{
	// проверить нужно ли пересканировать пути
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

bool CLocatorAPI::path_exist(const char* path)
{
	return pathes.find(path) != pathes.end();
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* fn)
{
	files_it it = file_find_it(fn);
	return (it != m_files.end()) ? &(*it) : 0;
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
	strconcat(sizeof(nm), nm, name, ext);
	update_path(fn, path, nm);
	return			exist(fn);
}

bool ignore_path(const char* _path) {
	HANDLE h = CreateFile(_path, 0, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING, NULL);

	if (h != INVALID_HANDLE_VALUE)
	{
		CloseHandle(h);
		return false;
	}
	else
		return true;
}

IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{
	return xr_strcmp(x.name, y.name)<0;
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
		Register(path, 0xffffffff, 0, 0, 0, 0, 0);
	return true;
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
		Register(N, 0xffffffff, 0, 0, entry.size, entry.size, (u32)entry.time_write);
		Recurse(N);
	}
	else {
		if (strext(N) && (!strncmp(strext(N), ".db", 3) || !strncmp(strext(N), ".xdb", 4)))
			ProcessArchive(N);
		else
			Register(N, 0xffffffff, 0, 0, entry.size, entry.size, (u32)entry.time_write);
	}
}

void  CLocatorAPI::rescan_pathes()
{
	m_Flags.set(flNeedRescan, FALSE);
	for (const auto& it : pathes)
	{
		FS_Path* P = it.second;
		if (P->m_Flags.is(FS_Path::flNeedRescan)) {
			rescan_path(P->m_Path, P->m_Flags.is(FS_Path::flRecurse));
			P->m_Flags.set(FS_Path::flNeedRescan, FALSE);
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
	if ((0 == m_iLockRescan) && m_Flags.is(flNeedRescan))
		rescan_pathes();
}

void CLocatorAPI::check_pathes()
{
	if (m_Flags.is(flNeedRescan) && (0 == m_iLockRescan)) {
		lock_rescan();
		rescan_pathes();
		unlock_rescan();
	}
}

void CLocatorAPI::Register(const char* name, u32 vfs, u32 crc, u32 ptr, u32 size_real, u32 size_compressed, u32 modif)
{
	//Msg("Register[%d] [%s]",vfs,name);
	string256			temp_file_name;
	xr_strcpy(temp_file_name, sizeof(temp_file_name), name);
	xr_strlwr(temp_file_name);

	// Register file
	file				desc;
	//	desc.name			= xr_strlwr(xr_strdup(name));
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
		_splitpath(temp, path, folder, 0, 0);
		xr_strcat(path, folder);
		if (!exist(path))
		{
			desc.name = xr_strdup(path);
			desc.vfs = 0xffffffff;
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


void CLocatorAPI::archive::close()
{
	CloseHandle(hSrcMap);
	hSrcMap = NULL;
	CloseHandle(hSrcFile);
	hSrcFile = NULL;
}

IReader* open_chunk(void* ptr, u32 ID)
{
	BOOL res;
	u32 dwType, dwSize;
	DWORD read_byte;
	u32 pt = SetFilePointer(ptr, 0, 0, FILE_BEGIN); VERIFY(pt != INVALID_SET_FILE_POINTER);
	while (true) {
		res = ReadFile(ptr, &dwType, 4, &read_byte, 0);
		if (read_byte == 0)
			return NULL;

		res = ReadFile(ptr, &dwSize, 4, &read_byte, 0);
		if (read_byte == 0)
			return NULL;

		if ((dwType&(~CFS_CompressMark)) == ID) {
			u8* src_data = xr_alloc<u8>(dwSize);
			res = ReadFile(ptr, src_data, dwSize, &read_byte, 0); VERIFY(res && (read_byte == dwSize));
			if (dwType&CFS_CompressMark) {
				BYTE*			dest;
				unsigned		dest_sz;
				_decompressLZ(&dest, &dest_sz, src_data, dwSize);
				xr_free(src_data);
				return new CTempReader(dest, dest_sz, 0);
			}
			else
				return new CTempReader(src_data, dwSize, 0);
		}
		else {
			pt = SetFilePointer(ptr, dwSize, 0, FILE_CURRENT);
			if (pt == INVALID_SET_FILE_POINTER) return 0;
		}
	}
	return 0;
};

void CLocatorAPI::ProcessArchive(const char* _path)
{
	// find existing archive
	shared_str path = _path;

	for (const auto& it : m_archives)
		if (it.path == path)
			return;

	m_archives.push_back(archive());
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

void CLocatorAPI::rescan_path(const char* full_path, BOOL bRecurse)
{
	file desc;
	desc.name = full_path;
	files_it	I = m_files.lower_bound(desc);
	if (I == m_files.end())	return;

	size_t base_len = xr_strlen(full_path);
	for (; I != m_files.end(); ) {
		files_it cur_item = I;
		const file& entry = *cur_item;
		I = cur_item; I++;
		if (0 != strncmp(entry.name, full_path, base_len))	break;	// end of list
		if (entry.vfs != 0xFFFFFFFF)						continue;
		const char* entry_begin = entry.name + base_len;
		if (!bRecurse&&strstr(entry_begin, "\\"))		continue;
		// erase item
		char* str = const_cast<char*>(cur_item->name);
		xr_free(str);
		m_files.erase(cur_item);
	}
	bNoRecurse = !bRecurse;
	Recurse(full_path);
}

void CLocatorAPI::archive::open()
{
	// Open the file
	if (hSrcFile && hSrcMap)
		return;

	hSrcFile = CreateFile(*path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	R_ASSERT(hSrcFile != INVALID_HANDLE_VALUE);
	hSrcMap = CreateFileMapping(hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
	R_ASSERT(hSrcMap != INVALID_HANDLE_VALUE);
	size = GetFileSize(hSrcFile, 0);
	R_ASSERT(size>0);
}

void CLocatorAPI::LoadArchive(archive& A, const char* entrypoint)
{
	// Create base path
	string_path					fs_entry_point;
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
				//				R_ASSERT3				(root, "path not found ", read_path.c_str());
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

		strconcat(sizeof(full), full, fs_entry_point, name);

		Register(full, A.vfs_idx, crc, ptr, size_real, size_compr, 0);
	}
	hdr->close();

	//	if(g_temporary_stuff_subst)
	//		g_temporary_stuff		= g_temporary_stuff_subst;
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(const char* fname)
{
	// проверить нужно ли пересканировать пути
	check_pathes();

	file			desc_f;
	string_path		file_name;
	VERIFY(xr_strlen(fname) * sizeof(char) < sizeof(file_name));
	xr_strcpy(file_name, sizeof(file_name), fname);
	desc_f.name = file_name;
	files_it I = m_files.find(desc_f);
	return			(I);
}
