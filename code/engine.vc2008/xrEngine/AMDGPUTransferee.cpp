//////////////////////////////////////////
// Desc:   GPU Info
// Model:  AMD
// Author: ForserX
//////////////////////////////////////////
// Oxygen Engine (2016-2019)
//////////////////////////////////////////

#include "stdafx.h"
#include "AMDGPUTransferee.h"

CAMDReader AMDData;

bool CAMDReader::bAMDSupportADL = false;

CAMDReader::CAMDReader() : 
	activity({ 0 }), 
	AdapterID(-1),
	AdapterAGSInfo(0),
	hAMDMain(NULL),
	hAMDAGS(NULL)
{}

CAMDReader::~CAMDReader()
{
	if (bAMDSupportADL)
	{
		if (hAMDMain != NULL)
		{
			Main_Control_Destroy();
			FreeLibrary(hAMDMain);
		}
		if (hAMDAGS != NULL)
		{
			FreeLibrary(hAMDAGS);
		}
	}
}

void CAMDReader::Initialize()
{
	if (bInitialized) return;
	hAMDMain = LoadLibraryA("atiadlxx.dll");

	if (hAMDMain != NULL)
	{
		auto TryInitializeAMDFunctionLambda = [this](void** pFunc, const char* FuncName) -> bool
		{
			*pFunc = GetProcAddress(hAMDMain, FuncName);
			if (*pFunc == nullptr)
			{
				FreeLibrary(hAMDMain);
				hAMDMain = NULL;
				Msg("! atiadlxx.dll doesn't have function \"%s\"", FuncName);
				return false;
			}

			return true;
		};

		if (!TryInitializeAMDFunctionLambda((void**)& Main_Control_Create, "ADL_Main_Control_Create"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&GetAdapter_Active, "ADL_Adapter_Active_Get"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&GetAdapter_NumberOfAdapters, "ADL_Adapter_NumberOfAdapters_Get"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&ADL_Adapter_AdapterInfo_Get, "ADL_Adapter_AdapterInfo_Get"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&GetOverdrive5_CurrentActivity, "ADL_Overdrive5_CurrentActivity_Get"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&Main_Control_Destroy, "ADL_Main_Control_Destroy"))
		{
			return;
		}
		if (!TryInitializeAMDFunctionLambda((void**)&GetTemperatureGPU, "ADL_Overdrive5_Temperature_Get"))
		{
			return;
		}

		bAMDSupportADL = true;

		Main_Control_Create(MemoryAllocator, 1);
		InitDeviceInfo();

		activity.iSize = sizeof(ADLPMActivity);
	}

	hAMDAGS = LoadLibraryA("amd_ags_x64.dll");
	if (hAMDAGS != NULL)
	{
		auto TryInitializeAMDAGSFunctionLambda = [this](void** pFunc, const char* FuncName) -> bool
		{
			*pFunc = GetProcAddress(hAMDAGS, FuncName);
			if (*pFunc == nullptr)
			{
				FreeLibrary(hAMDAGS);
				hAMDAGS = NULL;
				Msg("! amd_ags_x64.dll doesn't have function \"%s\"", FuncName);
				return false;
			}

			return true;
		};

		if (!TryInitializeAMDAGSFunctionLambda((void**)&AGSCrossfireInit, "agsInit"))
		{
			return;
		}
		if (!TryInitializeAMDAGSFunctionLambda((void**)&AGSCrossfireDeinit, "agsDeInit"))
		{
			return;
		}

		GetAGSCrossfireGPUCount = (AGS_GPU_COUNT_GET)GetProcAddress(hAMDAGS, "agsGetCrossfireGPUCount");
		if (GetAGSCrossfireGPUCount == nullptr)
		{
			if (!TryInitializeAMDAGSFunctionLambda((void**)&GetAGSCrossfireGPUCountExt, "agsDriverExtensionsDX11_CreateDevice"))
			{
				return;
			}
			if (!TryInitializeAMDAGSFunctionLambda((void**)&AGSCrossfireGPUExtDestroy, "agsDriverExtensionsDX11_DestroyDevice"))
			{
				return;
			}
		}

		MakeGPUCount();
	}

	bInitialized = true;
}

void CAMDReader::InitDeviceInfo()
{
	GetAdapter_NumberOfAdapters(&AdapterADLInfo);

	xrScopePtr< AdapterInfo > AdapterInfos;
	AdapterInfos.reset(new AdapterInfo[AdapterADLInfo]);

	// Get the AdapterInfo structure for all adapters in the system
	ADL_Adapter_AdapterInfo_Get(AdapterInfos.get(), sizeof(AdapterInfo) * AdapterADLInfo);

	for (u32 i = 0; i < (u32)AdapterADLInfo; i++)
	{
		int bAdapterActive = 0;
		AdapterInfo& adapterInfo = AdapterInfos[i];
		GetAdapter_Active(adapterInfo.iAdapterIndex, &bAdapterActive);
		if (bAdapterActive)
		{
			AdapterID = adapterInfo.iAdapterIndex;
			break;
		}
	}
}

void CAMDReader::MakeGPUCount()
{
	if (hAMDAGS == NULL)
	{
		return;
	}

	AGSContext* ags = nullptr;
	AGSGPUInfo gpuInfo = {};
	AGSConfiguration* config = nullptr;
	AGSReturnCode status = AGSCrossfireInit(&ags, config, &gpuInfo);

	if (status != AGS_SUCCESS)
	{
		Msg("! AGS: Initialization failed (%d)", status);
		return;
	}

	if (GetAGSCrossfireGPUCount)
	{
		// FX: Old style for Win7 and lazy users
		// But, it's just beautiful 
		status = GetAGSCrossfireGPUCount(ags, &AdapterAGSInfo);
		AdapterAGSInfo = AdapterAGSInfo ? AdapterAGSInfo : AdapterADLInfo / 2;
		Msg("[AGS] Used old ags driver...");
	}
	else
	{
		AGSDX11DeviceCreationParams creationParams;
		RtlZeroMemory(&creationParams, sizeof(AGSDX11DeviceCreationParams));
		creationParams.SDKVersion = 7; // Skip debug output errors. crossfireGPUCount need only
		creationParams.FeatureLevels = 45312; // 11.1

		AGSDX11ExtensionParams extensionParams = {};
		// FX: Enable AFR without requiring a driver profile
		extensionParams.crossfireMode = AGS_CROSSFIRE_MODE_EXPLICIT_AFR;
		extensionParams.uavSlot = 7;
		AGSDX11ReturnedParams returnedParams = {};

		GetAGSCrossfireGPUCountExt(ags, &creationParams, &extensionParams, &returnedParams);
		AdapterAGSInfo = returnedParams.crossfireGPUCount ? returnedParams.crossfireGPUCount : AdapterADLInfo / 2;

		AGSCrossfireGPUExtDestroy(ags, nullptr, nullptr, nullptr, nullptr);
	}

	if (status != AGS_SUCCESS) Msg("[AGS] Error! Unable to get CrossFire GPU count (%d)", status);
	else					   Msg("[AGS] CrossFire GPU count: %d", AdapterAGSInfo);

	AGSCrossfireDeinit(ags);
}

u32 CAMDReader::GetPercentActive()
{
	GetOverdrive5_CurrentActivity(AdapterID, &activity);
	return activity.iActivityPercent;
}

u32 CAMDReader::GetTemperature()
{
	ADLTemperature adlTemperature = { 0 };
	adlTemperature.iSize = sizeof(ADLTemperature);
	GetTemperatureGPU(AdapterID, 0, &adlTemperature);
	return u32(adlTemperature.iTemperature);
}

u32 CAMDReader::GetGPUCount()
{
	u32 uCount = hAMDAGS ? u32(AdapterAGSInfo) : u32(AdapterADLInfo / 2);
	return uCount ? uCount : 1;
}
