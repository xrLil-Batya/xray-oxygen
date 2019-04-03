#pragma once
#include <amd\adl_sdk.h>

class CAMDReader
{
	typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
	typedef int(*ADL_ADAPTER_ACTIVE_GET) (int, int*);
	typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
	typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
	typedef int(*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int iAdapterIndex, ADLPMActivity *lpActivity);
	typedef int(*ADL_MAIN_CONTROL_DESTROY)();

private:
	// Memory allocation function
	static void* __stdcall MemoryAllocator(int iSize)
	{
		lpBuffer = xr_malloc(iSize);
		return lpBuffer;
	}

	// Optional Memory de-allocation function
	static void MemoryDeallocator()
	{
		if (lpBuffer)
		{
			xr_free(lpBuffer);
			lpBuffer = nullptr;
		}
	}

private:
	ADL_MAIN_CONTROL_CREATE					Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY				Main_Control_Destroy;

	ADL_ADAPTER_ADAPTERINFO_GET				GetAdapter_AdapterInfo;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET		GetAdapter_NumberOfAdapters;
	ADL_ADAPTER_ACTIVE_GET					GetAdapter_Active;
	ADL_OVERDRIVE5_CURRENTACTIVITY_GET		GetOverdrive5_CurrentActivity;

	int				AdapterID;
	static void*	lpBuffer;
	ADLPMActivity	activity;

public:
	CAMDReader();
	~CAMDReader();

	void	InitDeviceInfo();
	u32		GetPercentActive();

public:
	static bool bAMDSupportADL;
};
