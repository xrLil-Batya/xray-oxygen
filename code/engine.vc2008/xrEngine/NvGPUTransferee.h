#pragma once
#include <NVAPI\nvapi.h>

#define NVAPI_MAX_USAGES_PER_GPU  34
class CNvReader
{
	typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
	typedef int(*NvAPI_Initialize_t)();
	typedef int(*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
	typedef int(*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);

private:
	NvAPI_QueryInterface_t      NvAPI_QueryInterface;
	NvAPI_Initialize_t          NvAPI_Initialize;
	NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs;
	NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages;

	int*	gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];
	u32		gpuUsages[NVAPI_MAX_USAGES_PER_GPU];
	int		AdapterID;

public:
	CNvReader();
	~CNvReader();

	void	InitDeviceInfo();
	u32		GetPercentActive();

public:
	static bool bSupport;
};
