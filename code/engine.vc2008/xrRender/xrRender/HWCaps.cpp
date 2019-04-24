#include "stdafx.h"
#pragma hdrstop

#include "hwcaps.h"
#include "hw.h"

#include "../../xrEngine/AMDGPUTransferee.h"
#include "../../xrEngine/NvGPUTransferee.h"

void CHWCaps::Update()
{
	// ***************** GEOMETRY
	geometry_major				= 4;
	geometry_minor				= 0;
	geometry.bSoftware			= FALSE;
	geometry.bPointSprites		= FALSE;
	geometry.bNPatches			= FALSE;
	DWORD cnt					= 256;
	clamp<DWORD>(cnt,0,256);
	geometry.dwRegisters		= cnt;
	geometry.dwInstructions		= 256;
	geometry.dwClipPlanes		= std::min(6,15);
	geometry.bVTF				= HW.IsFormatSupported(DXGI_FORMAT_R32_FLOAT);

	// ***************** PIXEL processing
	raster_major				= 4;
	raster_minor				= 0;
	raster.dwStages				= 16;
	raster.bNonPow2				= TRUE;
	raster.bCubemap				= TRUE;
	raster.dwMRT_count			= 4;
	raster.b_MRT_mixdepth		= TRUE;
	raster.dwInstructions		= 256;

	// ***************** Info
	Msg							("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)",
		0,	geometry_major, geometry_minor, CAP_VERSION(geometry_major,	geometry_minor),
		0,	raster_major,	raster_minor,	CAP_VERSION(raster_major,	raster_minor)
		);

	// *******1********** Vertex cache
	//	TODO: DX10: Find a way to detect cache size
	geometry.dwVertexCache = 24;
	Msg					("* GPU vertex cache: %s, %d","unrecognized",u32(geometry.dwVertexCache));

	// *******1********** Compatibility : vertex shader
	if (0==raster_major)		geometry_major=0;		// Disable VS if no PS

	//
	bTableFog			= FALSE;

	// Detect if stencil available
	bStencil			= TRUE;

	// Scissoring
	bScissor			= TRUE;

	// Stencil relative caps
	soInc				= D3D11_STENCIL_OP_INCR_SAT;
	soDec				= D3D11_STENCIL_OP_DECR_SAT;
	dwMaxStencilValue=(1<<8)-1;

	// DEV INFO
	if (CNvReader::bSupport)
	{
		id_vendor = 0x10DE;
		iGPUNum = NvData.GetGPUCount();
	}
	else if (CAMDReader::bAMDSupportADL)
	{
		id_vendor = 1002;
		iGPUNum = AMDData.GetGPUCount();
	}
	else
		iGPUNum = 1;
}