#include "stdafx.h"
#include "AMDGPUTransferee.h"

#define AMDVENDORID 1002

bool CAMDReader::bAMDSupportADL = false;
void* CAMDReader::lpBuffer = nullptr;

CAMDReader::CAMDReader() : activity({ 0 }), AdapterID(-1)
{
	static HINSTANCE hDLL = LoadLibraryA("atiadlxx.dll");
	if (!bAMDSupportADL && hDLL)
	{
		bAMDSupportADL = true;

		Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(hDLL, "ADL_Main_Control_Create");
		GetAdapter_NumberOfAdapters = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hDLL, "ADL_Adapter_NumberOfAdapters_Get");
		GetAdapter_AdapterInfo = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hDLL, "ADL_Adapter_AdapterInfo_Get");
		GetAdapter_Active = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL_Adapter_Active_Get");
		GetOverdrive5_CurrentActivity = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hDLL, "ADL_Overdrive5_CurrentActivity_Get");
		Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(hDLL, "ADL_Main_Control_Destroy");

		Main_Control_Create(MemoryAllocator, 1);
		InitDeviceInfo();

		activity.iSize = sizeof(ADLPMActivity);
	}
}

CAMDReader::~CAMDReader()
{
	Main_Control_Destroy();
	MemoryDeallocator();
	// FreeLibrary(hDLL);
}

void CAMDReader::InitDeviceInfo()
{
	LPAdapterInfo lpAdapterInfo = nullptr;
	int iNumberAdapters = 0;
	GetAdapter_NumberOfAdapters(&iNumberAdapters);

	lpAdapterInfo = new AdapterInfo[iNumberAdapters]();
	RtlZeroMemory(lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);

	// Get the AdapterInfo structure for all adapters in the system
	GetAdapter_AdapterInfo(lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);

	for (u32 i = 0; i < (u32)iNumberAdapters; i++)
	{
		int adapterActive = 0;
		AdapterInfo adapterInfo = lpAdapterInfo[i];
		GetAdapter_Active(adapterInfo.iAdapterIndex, &adapterActive);
		if (adapterActive && adapterInfo.iVendorID == AMDVENDORID)
		{
			AdapterID = adapterInfo.iAdapterIndex;
			break;
		}
	}
}

u32 CAMDReader::GetPercentActive()
{
	GetOverdrive5_CurrentActivity(AdapterID, &activity);
	return activity.iActivityPercent;
}
