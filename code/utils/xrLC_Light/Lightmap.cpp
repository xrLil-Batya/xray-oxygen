// Lightmap.cpp: implementation of the CLightmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "build.h"
#include "Lightmap.h"
#include "xrDeflector.h"
#include "xrDXTC.h"
#include "xrImage_Filter.h"
#include "xrface.h"
#include "serialize.h"
#include "ETextureParams.h"

#pragma comment(lib,"dxt.lib")
extern "C" bool __declspec(dllimport) __stdcall 
DXTCompress(LPCSTR out_name, u8* raw_data, u8* normal_map, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth);

CLightmap* CLightmap::read_create()
{
	return new CLightmap();
}

void CLightmap::Capture(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotated)
{
	// Allocate 512x512 texture if needed
	if (lm.surface.empty())	lm.create(c_LMAP_size, c_LMAP_size);

	// Addressing
	xr_vector<UVtri> tris;
	D->RemapUV(tris, b_u + BORDER, b_v + BORDER, s_u - 2 * BORDER, s_v - 2 * BORDER, c_LMAP_size, c_LMAP_size, bRotated);

	// Capture faces and setup their coords
	for (UVtri& tris_iter: tris)
	{
		Face *F = tris_iter.owner;
		F->lmap_layer = this;
		F->AddChannel(tris_iter.uv[0], tris_iter.uv[1], tris_iter.uv[2]);
	}

	// Perform BLIT
	lm_layer& L = D->layer;
	if (!bRotated)
	{
		u32 real_H = (L.height + 2 * BORDER);
		u32 real_W = (L.width + 2 * BORDER);
		blit(lm, c_LMAP_size, c_LMAP_size, L, real_W, real_H, b_u, b_v, 254 - BORDER);
	}
	else
	{
		u32 real_H = (L.height + 2 * BORDER);
		u32 real_W = (L.width + 2 * BORDER);
		blit_r(lm, c_LMAP_size, c_LMAP_size, L, real_W, real_H, b_u, b_v, 254 - BORDER);
	}
}

void CLightmap::Save(LPCSTR path)
{
	static int lmapNameID = 0; ++lmapNameID;

	Logger.Phase("Saving...");

	// Borders correction
	Logger.Status("Borders...");
	for (u32 _y = 0; _y<c_LMAP_size; _y++)
	{
		for (u32 _x = 0; _x<c_LMAP_size; _x++)
		{
			u32	offset = _y * c_LMAP_size + _x;
			if (lm.marker[offset] >= (254 - BORDER))	lm.marker[offset] = 255; else lm.marker[offset] = 0;
		}
	}
	for (u32 ref = 254; ref>(254 - 16); ref--) 
	{
		ApplyBorders(lm, ref);
		Logger.Progress(1.f - float(ref) / float(254 - 16));
	}
	Logger.Progress(1.f);

	xr_vector<u32> lm_packed;
	lm.Pack(lm_packed);
	xr_vector<u32> hemi_packed;
	lm.Pack_hemi(hemi_packed);

	lm_texture.bHasAlpha = TRUE;
	lm_texture.dwWidth = lm.width;
	lm_texture.dwHeight = lm.height;
	lm_texture.pSurface = NULL;

	lm.destroy();

	Logger.Status("Compression base...");
	{
		string_path				FN;
		xr_sprintf(lm_texture.name, "lmap#%d", lmapNameID);
		xr_sprintf(FN, "%s%s_1.dds", path, lm_texture.name);
		BYTE*	raw_data = LPBYTE(&*lm_packed.begin());
		u32	w = lm_texture.dwWidth;
		u32	h = lm_texture.dwHeight;
		u32	pitch = w * 4;

		STextureParams fmt;
		fmt.fmt = STextureParams::tfRGBA;
		fmt.flags.set(STextureParams::flDitherColor, FALSE);
		fmt.flags.set(STextureParams::flGenerateMipMaps, FALSE);
		fmt.flags.set(STextureParams::flBinaryAlpha, FALSE);
		DXTCompress(FN, raw_data, 0, w, h, pitch, &fmt, 4);
	}
	lm_packed.clear();
	Logger.Status("Compression hemi...");
	{
		u32 w = lm_texture.dwWidth;
		u32 h = lm_texture.dwHeight;
		u32	pitch = w * 4;

		string_path				FN;
		xr_sprintf(lm_texture.name, "lmap#%d", lmapNameID);
		xr_sprintf(FN, "%s%s_2.dds", path, lm_texture.name);
		BYTE*	raw_data = LPBYTE(&*hemi_packed.begin());

		STextureParams fmt;
		fmt.fmt = STextureParams::tfRGBA;
		fmt.flags.set(STextureParams::flDitherColor, FALSE);
		fmt.flags.set(STextureParams::flGenerateMipMaps, FALSE);
		fmt.flags.set(STextureParams::flBinaryAlpha, FALSE);
		DXTCompress(FN, raw_data, 0, w, h, pitch, &fmt, 4);
	}
}

void CLightmap::read(IReader	&r)
{
	lm.read(r);
	::read(r, lm_texture);

}
void CLightmap::write(IWriter	&w)const
{
	lm.write(w);
	::write(w, lm_texture);
}
