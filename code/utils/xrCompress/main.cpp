#include "stdafx.h"
#include "xrCompress.h"
#include "xrCompressWnd.h"
#include <iostream>

#ifndef MOD_COMPRESS
	extern int ProcessDifference();
#endif
	
extern const char* FolderName;
extern const char* KeysList;

void Startup(const char* argv)
{
	LPCSTR params = argv;

	xrCompressor C;

	if (strstr(params, "-delete"))
	{
		for (u32 iter = 0; iter < 21; iter++)
		{
			std::string name1 = "gamedata.db";
			std::string name2 = "gamedata.xdb";
			name1 += char(iter);
			name2 += char(iter);
			remove(name1.c_str());
			remove(name2.c_str());
		}
	}

	C.SetStoreFiles(NULL != strstr(params, "-store"));

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

		C.SetFastMode(NULL != strstr(params, "-fast"));
		C.SetTargetName(FolderName);

		MessageBox(0, "", FolderName, 0);
		LPCSTR p = strstr(params, "-ltx");

		if (0 != p)
		{
			string64 ltx_name;
			sscanf(strstr(params, "-ltx ") + 5, "%[^ ] ", ltx_name);

			CInifile ini(ltx_name);
			printf("Processing ...\n");
			C.ProcessLTX(ini);
		}
		else
		{
			string64 header_name;
			sscanf(strstr(params, "-header ") + 8, "%[^ ] ", header_name);
			C.SetPackHeaderName(header_name);
			C.ProcessTargetFolder();
		}
	}
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Debug._initialize(false);
	Core._initialize("xrCompress", 0, TRUE, "fsgame.ltx");
	// Initialize debugging
	xrCompress::Application::EnableVisualStyles();
	xrCompress::Application::SetCompatibleTextRenderingDefault(false);
	xrCompress::Application::Run(gcnew xrCompress::xrCompressWnd);

	Core._destroy();
	return 0;
}
