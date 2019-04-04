#include "stdafx.h"
#include "NvGPUTransferee.h"

bool CNvReader::bSupport = false;

CNvReader::CNvReader() : AdapterID(0)
{
	static HINSTANCE hDLL = LoadLibraryA("nvapi.dll");
	if (!bSupport && hDLL)
	{
		bSupport = true;

		NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(hDLL, "nvapi_QueryInterface");
		NvAPI_Initialize = (NvAPI_Initialize_t)(*NvAPI_QueryInterface)(0x0150E828);
		NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t)(*NvAPI_QueryInterface)(0xE5AC921F);
		NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t)(*NvAPI_QueryInterface)(0x189A1FDF);

		gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { nullptr };
		gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };
		InitDeviceInfo();

	}
}

CNvReader::~CNvReader()
{
	// FreeLibrary(hDLL);
}

void CNvReader::InitDeviceInfo()
{
	(*NvAPI_Initialize)();

	// gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
	gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

	(*NvAPI_EnumPhysicalGPUs)(gpuHandles, &AdapterID);
}

u32 CNvReader::GetPercentActive()
{
	(*NvAPI_GPU_GetUsages)(gpuHandles[0], gpuUsages);
	int usage = gpuUsages[3];
	return (u32)usage;
}
