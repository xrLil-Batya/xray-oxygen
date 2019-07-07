// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#pragma warning(disable:4995)
#include "../../xrEngine/stdafx.h"
#pragma warning(disable:4995 4005)
#include <d3dx9.h>
#pragma warning(default:4995 4005)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4237)
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <D3D11.h>
#include <D3Dx11core.h>
#include <D3DCompiler.h>

#include "../xrRender/xrD3DDefs.h"
#include "../xrRenderDX11/dx11EventWrapper.h"

#define		R_R1	1
#define		R_R2	2
#define		R_R3	3
#define		R_R4	4
#define		RENDER	R_R4

#ifdef XRRENDER_R4_EXPORTS
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif

#include "../../xrParticles/psystem.h"

#include "../xrRender/HW.h"
#include "../xrRender/Shader.h"
#include "../xrRender/R_Backend.h"
#include "../xrRender/R_Backend_Runtime.h"

#include "../xrRender/resourcemanager.h"

#include "../../xrEngine/vis_common.h"
#include "../../xrEngine/render.h"
#include "../../xrEngine/_d3d_extensions.h"
#include "../../xrEngine/igame_level.h"
#include "../xrRender/blenders\blender.h"
#include "../xrRender/blenders\blender_clsid.h"
#include "../xrRender/xrRender_console.h"
#include "r4.h"

inline void jitter(CBlender_Compile& C)
{
	C.r_dx10Texture	("jitter0",	JITTER(0));
	C.r_dx10Texture	("jitter1",	JITTER(1));
	C.r_dx10Texture	("jitter2",	JITTER(2));
	C.r_dx10Texture	("jitter3",	JITTER(3));
	C.r_dx10Texture	("jitter4",	JITTER(4));
	C.r_dx10Texture	("jitterMipped",	r2_jitter_mipped);
	C.r_dx10Sampler	("smp_jitter");
}

extern D3D11_USAGE CurrUsageVer;
extern UINT CurrBindFlag;