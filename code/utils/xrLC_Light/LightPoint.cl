//////////////////////////////////////////////////////
// Author: Giperion [EUREKA] 3.6.1
// Date:   August 2018
// Proj:   xrLC Redux Project
// Desc:   Hardware light support class for OpenCL
// Modifer: ForserX

//#WIP: OpenCL is bullshit

#include "../../xrCore/_types.h"
#include "xrRayDefinition.h"
#include "light_point.h"
#include "base_color.h"

__kernel void GenerateRaysForTask(
	__global xrHardwareLCGlobalData* GlobalData,
	__global RayRequest* RequestedRays,
	__global Ray* RayBuffer,
	__global u32* AliveRaysIndexes,
	__global u32 AliveRaysCount,
	bool CheckRGB, bool CheckSun, bool CheckHemi)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;

	// early exit condition
	if (idx >= AliveRaysCount) return;
	
	int RaysPerVertex = 0;
	if (CheckRGB)  RaysPerVertex += GlobalData->LightSize->RGBLightCount;
	if (CheckSun)  RaysPerVertex += GlobalData->LightSize->SunLightCount;
	if (CheckHemi) RaysPerVertex += GlobalData->LightSize->HemiLightCount;
}