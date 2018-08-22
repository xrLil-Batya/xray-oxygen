//Giperion August 2018
//[EUREKA] 3.6.1
//xrLC Redux Project
//Hardware light support class for OpenCL


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

}