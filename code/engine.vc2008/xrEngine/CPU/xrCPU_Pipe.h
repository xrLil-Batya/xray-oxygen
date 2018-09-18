#pragma once
#include "../bone.h"
#include "../device.h"
#include "../../xrRender/xrRender/SkeletonXVertRender.h"

class light;
// Skinning processor specific functions
// NOTE: Destination memory is uncacheble write-combining (AGP), so avoid non-linear writes
// D: AGP,			32b aligned
// S: SysMem		non-aligned
// Bones: SysMem	64b aligned

using xrSkin1W = void	__stdcall		(vertRender* D, vertBoned1W* S, u32 vCount, CBoneInstance* Bones);
using xrSkin2W = void	__stdcall		(vertRender* D, vertBoned2W* S, u32 vCount, CBoneInstance* Bones);
using xrSkin3W = void	__stdcall		(vertRender* D, vertBoned3W* S, u32 vCount, CBoneInstance* Bones);
using xrSkin4W = void	__stdcall		(vertRender* D, vertBoned4W* S, u32 vCount, CBoneInstance* Bones);

using xrPLC_calc3 = void	__stdcall		(int& c0, int& c1, int& c2, CRenderDevice& Device, Fvector* P, Fvector& N, light* L, float energy, Fvector& O);

#pragma pack(push,8)
struct xrDispatchTable
{
	xrSkin1W*			skin1W;
	xrSkin2W*			skin2W;
	xrSkin3W*			skin3W;
	xrSkin4W*			skin4W;
	xrPLC_calc3*		PLC_calc3;
};
#pragma pack(pop)

// Binder
// NOTE: Engine calls function named "_xrBindPSGP"
void xrBind_PSGP(xrDispatchTable* T, processor_info* ID);