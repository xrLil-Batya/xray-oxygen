#include "stdafx.h"
#include "LW_Shader.h"
#include "..\..\Shared\BlenderListLoader.h"

extern "C" { EShaderList ENShaders; EShaderList LCShaders; EShaderList GameMtls;}

extern "C" {
	void __cdecl LoadShaders()
	{
		Core._initialize("XRayPlugin", 0, FALSE);
		Core.SetPluginMode();
		
		FS._initialize(CLocatorAPI::flScanAppRoot, NULL, "xray_path.ltx");
		LPSTRVec lst;
		ENShaders.count = LoadBlenderList(lst);
		for (char* &b_it : lst)
			strcpy(ENShaders.Names[b_it - *lst.begin()], b_it);
		ClearList(lst);

		LCShaders.count = LoadShaderLCList(lst);
		for (char* &c_it : lst)
			strcpy(LCShaders.Names[c_it - *lst.begin()], c_it);
		ClearList(lst);

		GameMtls.count = LoadGameMtlList(lst);
		for (char* &g_it : lst)
			strcpy(GameMtls.Names[g_it - *lst.begin()], g_it);
		ClearList(lst);
	}
};