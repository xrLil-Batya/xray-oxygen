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

CLocatorAPI*	xr_FS	= nullptr;

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
	// ignore processing ".svn" folders
	return ( _name[0]=='.' && _name[1]=='s' && _name[2]=='v' && _name[3]=='n' && _name[4]==0) ;
}

typedef void	(__stdcall * TOnFind)	(_finddata_t&, void*);
void Recurse	(LPCSTR, bool, TOnFind, void*);
void ProcessOne	(LPCSTR path, _finddata_t& F, bool root_only, TOnFind on_find_cb, void* data)
{
	string_path	N;
	strcpy		(N,path);
	strcat		(N,F.name);
	xr_strlwr	(N);

	if (ignore_name(N))					return;
    
	if (F.attrib&_A_HIDDEN)				return;

	if (F.attrib&_A_SUBDIR) {
    	if (root_only)					return;
		if (0==xr_strcmp(F.name,"."))	return;
		if (0==xr_strcmp(F.name,"..")) 	return;
		strcat		(N,"\\");
	    strcpy		(F.name,N);
        on_find_cb	(F,data);
		Recurse		(F.name,root_only,on_find_cb,data);
	} else {
	    strcpy		(F.name,N);
        on_find_cb	(F,data);
	}
}

void Recurse(LPCSTR path, bool root_only, TOnFind on_find_cb, void* data)
{
	xr_string		fpath	= path;
	fpath			+= "*.*";

    // begin search
    _finddata_t		sFile;
    intptr_t		hFile;

	// find all files    
	if (-1==(hFile=_findfirst((LPSTR)fpath.c_str(), &sFile)))
    	return;
    do{
    	ProcessOne	(path,sFile, root_only, on_find_cb, data);
    }while(_findnext(hFile,&sFile)==0);
	_findclose		( hFile );
}

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
