#include "stdafx.h"
#include "../../xrEngine/customhud.h"

float				g_fSCREEN		;

extern float		r_dtex_range	;
extern float		r_ssaDISCARD	;
extern float		r_ssaDONTSORT	;
extern float		r_ssaLOD_A		;
extern float		r_ssaLOD_B		;
extern float		r_ssaHZBvsTEX	;
extern float		r_ssaGLOD_start,	r_ssaGLOD_end;

void CRender::Calculate()
{
	// Transfer to global space to avoid deep pointer access
	IRender_Target* T = getTarget();
	float	fov_factor = _sqr(90.f / Device.fFOV);
	g_fSCREEN = float(T->get_width()*T->get_height())*fov_factor*(EPS_S + ps_r_LOD);
	r_ssaDISCARD = _sqr(ps_r_ssaDISCARD) / g_fSCREEN;
	r_ssaDONTSORT = _sqr(ps_r_ssaDONTSORT / 3) / g_fSCREEN;
	r_ssaLOD_A = _sqr(ps_r_ssaLOD_A / 3) / g_fSCREEN;
	r_ssaLOD_B = _sqr(ps_r_ssaLOD_B / 3) / g_fSCREEN;
	r_ssaGLOD_start = _sqr(ps_r_GLOD_ssa_start / 3) / g_fSCREEN;
	r_ssaGLOD_end = _sqr(ps_r_GLOD_ssa_end / 3) / g_fSCREEN;
	r_ssaHZBvsTEX = _sqr(ps_r_ssaHZBvsTEX / 3) / g_fSCREEN;
	r_dtex_range = ps_r_df_parallax_range * g_fSCREEN / (1024.f * 768.f);

	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition, EPS_L))
	{
		CSector* pSector = (CSector*)detectSector(Device.vCameraPosition);
		if (pSector && (pSector != pLastSector))
			g_pGamePersistent->OnSectorChanged(translateSector(pSector));

		if (0 == pSector) pSector = pLastSector;
		pLastSector = pSector;
		vLastCameraPos.set(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
#if 0
	if (rmPortals)
	{
		float	eps = VIEWPORT_NEAR + EPS_L;
		Fvector box_radius; box_radius.set(eps, eps, eps);
		Sectors_xrc.box_options(CDB::OPT_FULL_TEST);
		Sectors_xrc.box_query(rmPortals, Device.vCameraPosition, box_radius);

		if (!Sectors_xrc.r_empty())
		{
			for (auto sectorIter = Sectors_xrc.r_realBegin(); sectorIter != Sectors_xrc.r_realEnd(); sectorIter++)
			{
				CDB::TRI& targetTriangle = rmPortals->get_tris()[sectorIter->id];
				CPortal* pPortal = (CPortal*)Portals[targetTriangle.dummy];
			}
		}
	}
#endif

	// Update
	Lights.Update();

	// Check if we touch some light even trough portal
	lstRenderables.clear();
	g_SpatialSpace->q_sphere(lstRenderables, 0, STYPE_LIGHTSOURCE, Device.vCameraPosition, EPS_L);
	for (ISpatial*	spatial : lstRenderables)
	{
		spatial->spatial_updatesector();

		// disassociated from S/P structure
		if (0 == (CSector*)spatial->spatial.sector) continue;	

		VERIFY(spatial->spatial.type & STYPE_LIGHTSOURCE);

		// lightsource
		light* pLight = (light*)(spatial->dcast_Light());
		VERIFY(pLight);
		Lights.add_light(pLight);
	}
}
