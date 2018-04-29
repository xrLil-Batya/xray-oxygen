#include "stdafx.h"

#include "../xrRender/du_sphere.h"
#include "../xrRenderDX10/dx10BufferUtils.h"

void CRenderTarget::accum_point_geom_create()
{
	// vertices
	{
		u32		vCount		= DU_SPHERE_NUMVERTEX;
		u32		vSize		= 3*4;

		R_CHK( dx10BufferUtils::CreateVertexBuffer	( &g_accum_point_vb, du_sphere_vertices, vCount*vSize ) );
		HW.stats_manager.increment_stats_vb			( g_accum_point_vb );
	}

	// Indices
	{
		u32		iCount		= DU_SPHERE_NUMFACES*3;

		R_CHK( dx10BufferUtils::CreateIndexBuffer	( &g_accum_point_ib, du_sphere_faces, iCount*2) );
		HW.stats_manager.increment_stats_ib			( g_accum_point_ib );
	}
}

void CRenderTarget::accum_point_geom_destroy()
{
#ifdef DEBUG
	_SHOW_REF("g_accum_point_ib",g_accum_point_ib);
#endif // DEBUG

	HW.stats_manager.decrement_stats_ib(g_accum_point_ib);
	_RELEASE(g_accum_point_ib);
#ifdef DEBUG
	_SHOW_REF("g_accum_point_vb",g_accum_point_vb);
#endif // DEBUG
	HW.stats_manager.decrement_stats_vb(g_accum_point_vb);
	_RELEASE(g_accum_point_vb);
}
