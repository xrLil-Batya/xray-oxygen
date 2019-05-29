// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "Environment.h"
#include "editor_environment_manager.hpp"
#include "engine_impl.hpp"

extern ENGINE_API CEnvironment* pEnvironment;
engine_impl* pEngine;

__declspec(dllexport) void InitEditor()
{
	pEngine = new engine_impl();
	Device.initialize_editor(pEngine);
	pEnvironment = new editor::environment::manager();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		{
			xr_delete(pEngine);
			break;
		}
	}
    return TRUE;
}

