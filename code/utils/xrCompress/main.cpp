#include "stdafx.h"
#include "xrCompress.h"
#include "xrCompressWnd.h"
#include <iostream>

#ifndef MOD_COMPRESS
	extern int ProcessDifference();
#endif
	
extern const char* FolderName;
extern const char* KeysList;
bool SecondThreadWorking = true;

void Startup(void*)
{
	xrCompressor C;

	C.SetStoreFiles(NULL != strstr(KeysList, "-store"));

#ifdef MOD_COMPRESS
	if (strstr(params, "-diff"))
	{
		ProcessDifference();
	}
	else
#endif

#ifdef MOD_XDB
	if (strstr(params, "-pack"))
#endif
	{
		FS._initialize(CLocatorAPI::flTargetFolderOnly, FolderName);
		FS.append_path("$target_folder$", "", 0, false);

		C.SetFastMode(NULL != strstr(KeysList, "-fast"));
		C.SetTargetName(FolderName);

		LPCSTR p = strstr(KeysList, "-ltx");

		if (0 != p)
		{
			string64 ltx_name;
			sscanf(strstr(KeysList, "-ltx ") + 5, "%[^ ] ", ltx_name);

			CInifile ini(ltx_name);
			printf("Processing ...\n");
			C.ProcessLTX(ini);
		}
		else
		{
			string64 header_name;
			sscanf(strstr(KeysList, "-header ") + 8, "%[^ ] ", header_name);
			C.SetPackHeaderName(header_name);
			C.ProcessTargetFolder();
		}
	}
	SecondThreadWorking = false;
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Debug._initialize();
	Core._initialize("xrCompress", 0, TRUE, "fsgame.ltx");
	// Initialize debugging
	xrCompress::Application::EnableVisualStyles();
	xrCompress::Application::SetCompatibleTextRenderingDefault(false);
	xrCompress::Application::Run(gcnew xrCompress::xrCompressWnd);

	Core._destroy();
	return 0;
}
