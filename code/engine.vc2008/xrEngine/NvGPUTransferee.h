#pragma once
#include <NVAPI\nvapi.h>

#define NVAPI_MAX_USAGES_PER_GPU  34
class ENGINE_API CNvReader
{
	using NvAPI_QueryInterface_t = int *(*)(unsigned int offset);
	using NvAPI_Initialize_t = int(*)();
	using NvAPI_EnumPhysicalGPUs_t = int(*)(int **handles, unsigned long *count);
	using NvAPI_EnumLogicalGPUs_t = int(*)(int **handles, unsigned long *count);
	using NvAPI_GPU_GetUsages_t = int(*)(int *handle, unsigned int *usages);
	using NvAPI_PhysicalFromLogical = int(*)(int* handle1, int** handle, unsigned long* count);

private:
	NvAPI_QueryInterface_t      NvAPI_QueryInterface;
	NvAPI_Initialize_t          NvAPI_Initialize;
	NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs;
	NvAPI_EnumLogicalGPUs_t     NvAPI_EnumLogicalGPUs;
	NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages;
	NvAPI_PhysicalFromLogical   NvAPI_GPU_PhysicalFromLogical;

	int*	gpuHandlesPh[NVAPI_MAX_PHYSICAL_GPUS];
	int*	gpuHandlesLg[NVAPI_MAX_LOGICAL_GPUS];
	u32		gpuUsages[NVAPI_MAX_USAGES_PER_GPU];
	ULONG	AdapterID;
	u64		AdapterFinal;

	HMODULE hNvAPIDLL;

private:
	void	InitDeviceInfo();
	void	MakeGPUCount();

public:
	CNvReader();
	~CNvReader();

	void	Initialize();
	u32		GetPercentActive();
	u32		GetGPUCount();

public:
	static bool bSupport;
};
