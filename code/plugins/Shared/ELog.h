//----------------------------------------------------
// file: Log.h
//----------------------------------------------------
#pragma once
class CLog
{
public:
	bool 		in_use;
public:
				CLog	(){in_use=false;}
	void 		Msg   	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...);
};

void ELogCallback(LPCSTR txt);

extern CLog ELog;
