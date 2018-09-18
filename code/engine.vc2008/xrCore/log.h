#pragma once
#define VPUSH(a)	((a).x), ((a).y), ((a).z)

void 	XRCORE_API	__cdecl		Msg	(const char* format, ...);
void 	XRCORE_API		Log			(const char* msg);
void 	XRCORE_API		Log			(const char* msg);
void 	XRCORE_API		Log			(const char* msg, const char*			dop);
void 	XRCORE_API		Log			(const char* msg, u32			dop);
void 	XRCORE_API		Log			(const char* msg, int  			dop);
void 	XRCORE_API		Log			(const char* msg, float			dop);
void 	XRCORE_API		Log			(const char* msg, const Fvector& dop);
void 	XRCORE_API		Log			(const char* msg, const Fmatrix& dop);
void 	XRCORE_API 		Log			(const char* msg, const unsigned long long dop);
void 	XRCORE_API		LogWinErr	(const char* msg, long 			err_code);
void	XRCORE_API		LogXrayOffset(const char* key, LPVOID base, LPVOID pval); //luaicp only
using LogCallback = void	( *)	(const char* string);
LogCallback	XRCORE_API			SetLogCB	(LogCallback cb);
void 	XRCORE_API				CreateLog	(BOOL no_log=FALSE);
void 	XRCORE_API				InitLog		();
void 							CloseLog	();
void	XRCORE_API				FlushLog	();

static string_path				lua_log_file_name;

extern 	XRCORE_API	xr_vector<shared_str>*		LogFile;
extern 	XRCORE_API	BOOL						LogExecCB;
