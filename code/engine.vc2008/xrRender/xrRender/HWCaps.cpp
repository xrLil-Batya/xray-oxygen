#include "stdafx.h"
#pragma hdrstop

#include "hwcaps.h"
#include "hw.h"

#include "../../xrEngine/AMDGPUTransferee.h"
#include "../../xrEngine/NvGPUTransferee.h"

bool CHWCaps::IsNvidiaCard() const
{
	constexpr UINT NvidiaVendorID = 0x10DE;
	return id_vendor == NvidiaVendorID;
}

bool CHWCaps::IsAMDCard() const
{
	constexpr UINT AMD1VendorID = 0x1002;
	constexpr UINT AMD2VendorID = 0x1022;
	return id_vendor == AMD1VendorID || id_vendor == AMD2VendorID;
}

bool CHWCaps::IsIntelCard() const
{
	constexpr UINT Intel1VendorID = 0x163C;
	constexpr UINT Intel2VendorID = 0x8086;
	constexpr UINT Intel3VendorID = 0x8087;
	return id_vendor == Intel1VendorID || id_vendor == Intel2VendorID || id_vendor == Intel3VendorID;
}

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
	DXGI_ADAPTER_DESC videoAdapterDesc;
	ZeroMemory(&videoAdapterDesc, sizeof(videoAdapterDesc));
	if (SUCCEEDED(HW.m_pAdapter->GetDesc(&videoAdapterDesc)))
	{
		id_vendor = videoAdapterDesc.VendorId;

		if (IsNvidiaCard())
		{
			NvidiaSpecific.Initialize();
		}
		else if (IsAMDCard())
		{
			AMDSpecific.Initialize();
		}
	}
	else
	{
		Msg("* HW.m_pAdapter->GetDesc failed!");
	}

	if (CNvReader::bSupport)
	{
		iGPUNum = NvidiaSpecific.GetGPUCount();
	}
	else if (CAMDReader::bAMDSupportADL)
	{
		iGPUNum = AMDSpecific.GetGPUCount();
	}
	else
		iGPUNum = 1;
}