#include "stdafx.h"

#include "fhierrarhyvisual.h"
#include "SkeletonCustom.h"
#include "../../xrEngine/fmesh.h"
#include "../../xrEngine/irenderable.h"

#include "flod.h"
#include "particlegroup.h"
#include "FTreeVisual.h"

using	namespace R_dsgraph;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
float		r_ssaDISCARD;
float		r_ssaDONTSORT;
float		r_ssaLOD_A,			r_ssaLOD_B;
float		r_ssaGLOD_start,	r_ssaGLOD_end;
float		r_ssaHZBvsTEX;

ICF float CalcSSA(float& distSQ, Fvector& C, dxRender_Visual* V)
{
	float R = V->vis.sphere.R + 0;
	distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
	return R / distSQ;
}
ICF float CalcSSA(float& distSQ, Fvector& C, float R)
{
	distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
	return R / distSQ;
}

R_dsgraph_structure::R_dsgraph_structure()
{
	val_pObject = NULL;
	val_pTransform = NULL;
	val_bHUD = false;
	val_bInvisible = false;
	val_bRecordMP = false;
	val_feedback = 0;
	val_feedback_breakp = 0;
	marker = 0;
	r_pmask(true, true);
	b_loaded = false;

	InsertStaticCritsect.Unlock();
}

void R_dsgraph_structure::r_dsgraph_destroy()
{
	while (InsertStaticCritsect.TryLock())
		Sleep(1);

	nrmVS.clear();
	nrmPS.clear();
	nrmCS.clear();
	nrmStates.clear();
	nrmTextures.clear();
	nrmTexturesTemp.clear();

	matVS.clear();
	matPS.clear();
	matCS.clear();
	matStates.clear();
	matTextures.clear();
	matTexturesTemp.clear();

	lstLODs.clear();
	lstLODgroups.clear();
	lstRenderables.clear();
	lstSpatial.clear();

	for (u32 i = 0; i < 2; ++i)
	{
		for (u32 j = 0; j < SHADER_PASSES_MAX; j++)
		{
			mapNormalPasses[i][j].clear();
			mapMatrixPasses[i][j].clear();
		}
	}
	mapSorted.clear();
	mapHUD.clear();
	mapLOD.clear();
	mapDistort.clear();

	mapWmark.clear();
	mapEmissive.clear();
}

void R_dsgraph_structure::r_dsgraph_insert_dynamic(dxRender_Visual* pVisual, Fvector& Center)
{
	CRender& RI = RImplementation;

	if (pVisual->vis.marker == RI.marker)	return;
	pVisual->vis.marker = RI.marker;

	float distSQ;
	float SSA = CalcSSA(distSQ, Center, pVisual);
	if (SSA <= r_ssaDISCARD)		return;

	// Distortive geometry should be marked and R2 special-cases it
	// a) Allow to optimize RT order
	// b) Should be rendered to special distort buffer in another pass
	VERIFY(pVisual->shader._get());
	ShaderElement* sh_d = &*pVisual->shader->E[4];
	if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority / 2])
	{
		_MatrixItemS temp;
		temp.ssa = SSA;
		temp.pObject = RI.val_pObject;
		temp.pVisual = pVisual;
		temp.Matrix = *RI.val_pTransform;
		temp.se = sh_d;
		mapDistort.emplace_back(std::make_pair(distSQ, temp));
	}

	// Select shader
	ShaderElement* sh = RImplementation.rimp_select_sh_dynamic(pVisual, distSQ);
	if (0 == sh)								return;
	if (!pmask[sh->flags.iPriority / 2])		return;

	// HUD rendering
	if (RI.val_bHUD)
	{
		if (sh->flags.bStrictB2F)
		{
			_MatrixItemS temp;
			temp.ssa = SSA;
			temp.pObject = RI.val_pObject;
			temp.pVisual = pVisual;
			temp.Matrix = *RI.val_pTransform;
			temp.se = sh;

			mapHUDSorted.emplace_back(std::make_pair(distSQ, temp));
			return;
		}
		else
		{
			_MatrixItemS temp;
			temp.ssa = SSA;
			temp.pObject = RI.val_pObject;
			temp.pVisual = pVisual;
			temp.Matrix = *RI.val_pTransform;
			temp.se = sh;
			mapHUD.emplace_back(std::make_pair(distSQ, temp));

			if (sh->flags.bEmissive)
			{
				_MatrixItemS temp1;
				temp1.ssa = SSA;
				temp1.pObject = RI.val_pObject;
				temp1.pVisual = pVisual;
				temp1.Matrix = *RI.val_pTransform;
				temp1.se = &*pVisual->shader->E[4];		// 4=L_special
				mapHUDEmissive.emplace_back(std::make_pair(distSQ, temp1));
			}
			return;
		}
	}

	// Shadows registering
	if (RI.val_bInvisible)		return;

	// strict-sorting selection
	if (sh->flags.bStrictB2F)
	{
		_MatrixItemS temp;
		temp.ssa = SSA;
		temp.pObject = RI.val_pObject;
		temp.pVisual = pVisual;
		temp.Matrix = *RI.val_pTransform;
		temp.se = sh;
		mapSorted.emplace_back(std::make_pair(distSQ, temp));
		return;
	}

	// Emissive geometry should be marked and R2 special-cases it
	// a) Allow to skeep already lit pixels
	// b) Allow to make them 100% lit and really bright
	// c) Should not cast shadows
	// d) Should be rendered to accumulation buffer in the second pass
	if (sh->flags.bEmissive)
	{
		_MatrixItemS temp;
		temp.ssa = SSA;
		temp.pObject = RI.val_pObject;
		temp.pVisual = pVisual;
		temp.Matrix = *RI.val_pTransform;
		temp.se = &*pVisual->shader->E[4];		// 4=L_special
		mapEmissive.emplace_back(std::make_pair(distSQ, temp));
	}

	if (sh->flags.bWmark && pmask_wmark)
	{
		_MatrixItemS temp;
		temp.ssa = SSA;
		temp.pObject = RI.val_pObject;
		temp.pVisual = pVisual;
		temp.Matrix = *RI.val_pTransform;
		temp.se = sh;
		mapWmark.emplace_back(std::make_pair(distSQ, temp));
		return;
	}

	_MatrixItem item = { SSA, RI.val_pObject, pVisual, *RI.val_pTransform };
	for (u32 iPass = 0; iPass < sh->passes.size(); ++iPass)
	{
		// the most common node
		SPass& pass = *sh->passes[iPass];
		auto& map = mapMatrixPasses[sh->flags.iPriority / 2][iPass];

		auto& Nvs = map[&*pass.vs];
		auto& Ngs = Nvs[pass.gs->gs];
		auto& Nps = Ngs[pass.ps->ps];
		Nps.hs = pass.hs->sh;
		Nps.ds = pass.ds->sh;

		auto& Ncs = Nps.mapCS[pass.constants._get()];
		auto& Nstate = Ncs[&pass.state];
		auto& Ntex = Nstate[pass.T._get()];
		Ntex.push_back(item);

		// Need to sort for HZB efficient use// Need to sort for HZB efficient use
		if (SSA > Ntex.ssa)
		{
			Ntex.ssa = SSA;
			if (SSA > Nstate.ssa)
			{
				Nstate.ssa = SSA;
				if (SSA > Ncs.ssa)
				{
					Ncs.ssa = SSA;
					if (SSA > Nps.mapCS.ssa)
					{
						Nps.mapCS.ssa = SSA;
						if (SSA > Ngs.ssa)
						{
							Ngs.ssa = SSA;
							if (SSA > Nvs.ssa)
							{
								Nvs.ssa = SSA;
							}
						}
					}
				}
			}
		}
	}
}

void R_dsgraph_structure::r_dsgraph_insert_static	(dxRender_Visual *pVisual)
{
	if (pVisual->vis.marker == RImplementation.marker)	return;

	CRender& RI					= RImplementation;
	pVisual->vis.marker			= RI.marker;

	float distSQ;
	float SSA					= CalcSSA(distSQ,pVisual->vis.sphere.P,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Distortive geometry should be marked and R2 special-cases it
	// a) Allow to optimize RT order
	// b) Should be rendered to special distort buffer in another pass
	VERIFY						(pVisual->shader._get());
	ShaderElement*		sh_d	= &*pVisual->shader->E[4];

	if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority/2]) {
		_MatrixItemS temp; 
		temp.ssa = SSA;
		temp.pObject = NULL;
		temp.pVisual = pVisual;
		temp.Matrix = Fidentity;
		temp.se = sh_d;		// 4=L_special
		mapDistort.emplace_back(std::make_pair(distSQ, temp));
	}

	// Select shader
	ShaderElement*		sh		= RImplementation.rimp_select_sh_static(pVisual,distSQ);
	if (0==sh)								return;
	if (!pmask[sh->flags.iPriority/2])		return;

	// strict-sorting selection
	if (sh->flags.bStrictB2F) 
	{
		_MatrixItemS temp;
		temp.pObject = NULL;
		temp.pVisual = pVisual;
		temp.Matrix = Fidentity;
		temp.se = sh;
		mapSorted.emplace_back(std::make_pair(distSQ, temp));

		return;
	}

	// Emissive geometry should be marked and R2 special-cases it
	// a) Allow to skip already lit pixels
	// b) Allow to make them 100% lit and really bright
	// c) Should not cast shadows
	// d) Should be rendered to accumulation buffer in the second pass
	if (sh->flags.bEmissive) 
	{
		_MatrixItemS temp; 
		temp.ssa = SSA;
		temp.pObject = NULL;
		temp.pVisual = pVisual;
		temp.Matrix = Fidentity;
		temp.se = &*pVisual->shader->E[4];		// 4=L_special
		mapEmissive.emplace_back(std::make_pair(distSQ, temp));
	}
	if (sh->flags.bWmark && pmask_wmark)
	{
		_MatrixItemS temp;
		temp.ssa = SSA;
		temp.pObject = nullptr;
		temp.pVisual = pVisual;
		temp.Matrix = Fidentity;
		temp.se = sh;
		mapWmark.emplace_back(std::make_pair(distSQ, temp));
		return;
	}

	if (val_feedback && counter_S == val_feedback_breakp)
	{
		val_feedback->rfeedback_static(pVisual);
	}
	counter_S++;

	_NormalItem item = { SSA, pVisual };
	for (u32 iPass = 0; iPass<sh->passes.size(); ++iPass)
	{
		SPass& pass	= *sh->passes[iPass];
		mapNormal_T& map = mapNormalPasses[sh->flags.iPriority/2][iPass];

		R_dsgraph::mapNormalGS& Nvs = map[&*pass.vs];
		R_dsgraph::mapNormalPS& Ngs = Nvs[pass.gs->gs];
		R_dsgraph::mapNormalAdvStages& Nps = Ngs[pass.ps->ps];

		Nps.hs = pass.hs->sh;
		Nps.ds = pass.ds->sh;

		auto &Ncs = Nps.mapCS[pass.constants._get()];
		auto &Nstate = Ncs[&pass.state];
		auto &Ntex = Nstate[pass.T._get()];
		Ntex.push_back(item);

		// Need to sort for HZB efficient use// Need to sort for HZB efficient use
		if (SSA > Ntex.ssa)
		{
			Ntex.ssa = SSA;
			if (SSA > Nstate.ssa)
			{
				Nstate.ssa = SSA;
				if (SSA > Ncs.ssa)
				{
					Ncs.ssa = SSA;
					if (SSA > Nps.mapCS.ssa)
					{
						Nps.mapCS.ssa = SSA;

						if (SSA > Ngs.ssa)
						{
							Ngs.ssa = SSA;

							if (SSA > Nvs.ssa)
								Nvs.ssa = SSA;
						}
					}
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr float O_D_L1_S_ULT = 2.0f;
constexpr float O_D_L1_D_ULT = 30.f;
constexpr float O_D_L2_S_ULT = 8.f;
constexpr float O_D_L2_D_ULT = 50.f;
constexpr float O_D_L3_S_ULT = 4000.f;
constexpr float O_D_L3_D_ULT = 110.f;

constexpr float O_D_L1_S_MED = 1.4f;
constexpr float O_D_L1_D_MED = 30.f;
constexpr float O_D_L2_S_MED = 4.f;
constexpr float O_D_L2_D_MED = 80.f;
constexpr float O_D_L3_S_MED = 4000.f;
constexpr float O_D_L3_D_MED = 150.f;

// Cut off Dynamic geometry depending of size of geometryand distance to cameraand current geometry optimization settings
IC bool IsValuableToRenderDyn(dxRender_Visual* pVisual, Fmatrix& transform_matrix, bool sm)
{
	auto GetDistFromCamera = [](const Fvector & from_position)
	{
		float distance = Device.vCameraPosition.distance_to(from_position);
		float fov_K = 70.f / Device.fFOV;
		float adjusted_distane = distance / fov_K;

		return adjusted_distane;
	};

	if (sm && ps_r_smapsize < 4096)
	{
		float sphere_volume = pVisual->getVisData().sphere.volume();

		Fvector    Tpos;
		transform_matrix.transform_tiny(Tpos, pVisual->vis.sphere.P);

		float adjusted_distane = GetDistFromCamera(Tpos);

		if (sm && ps_r_smapsize < 4096) // Highest cut off for shadow map
		{
			if (adjusted_distane > 160) // don't need geometry behind the farest sun shadow cascade
				return false;

			if ((sphere_volume < O_D_L1_S_ULT) && adjusted_distane > O_D_L1_D_ULT)
				return false;
			else if ((sphere_volume < O_D_L2_S_ULT) && adjusted_distane > O_D_L2_D_ULT)
				return false;
			else if ((sphere_volume < O_D_L3_S_ULT) && adjusted_distane > O_D_L3_D_ULT)
				return false;
			else
				return true;
		}
		else
		{
			if ((sphere_volume < O_D_L1_S_MED) && adjusted_distane > O_D_L1_D_MED)
				return false;
			else if ((sphere_volume < O_D_L2_S_MED) && adjusted_distane > O_D_L2_D_MED)
				return false;
			else if ((sphere_volume < O_D_L3_S_MED) && adjusted_distane > O_D_L3_D_MED)
				return false;
			else
				return true;
		}
	}

	return true;
}

void CRender::add_leafs_Dynamic	(dxRender_Visual *pVisual, bool bIgnoreOpt)
{
	if (pVisual == nullptr) return;

	// Visual is 100% visible - simply add it
	if (!bIgnoreOpt && !IsValuableToRenderDyn(pVisual, *val_pTransform, phase == PHASE_SMAP))
		return;

	switch (pVisual->Type)
	{
	case MT_PARTICLE_GROUP:
		{
			if (phase == PHASE_SMAP) return;
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG	= (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItem& I : pG->items)	
			{
				if (I._effect)
					add_leafs_Dynamic(I._effect, bIgnoreOpt);

				for (dxRender_Visual* pChildRelated : I._children_related)
					add_leafs_Dynamic(pChildRelated, bIgnoreOpt);

				for (dxRender_Visual* pChildFree : I._children_free)
					add_leafs_Dynamic(pChildFree, bIgnoreOpt);
			}
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;

			for (dxRender_Visual* pChildVisual : pV->children)
			{
				pChildVisual->vis.obj_data = pV->getVisData().obj_data;
				add_leafs_Dynamic(pChildVisual);
			}
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
		CKinematics* pV = (CKinematics*)pVisual;
		bool	_use_lod = false;
			if (pV->m_lod)				
			{
				Fvector							Tpos;	float		D;
				val_pTransform->transform_tiny	(Tpos, pV->vis.sphere.P);
				float		ssa		=	CalcSSA	(D,Tpos,pV->vis.sphere.R/2.f);	// assume dynamics never consume full sphere
				if (ssa<r_ssaLOD_A)	_use_lod	= true;
			}

			if (_use_lod)
			{
				add_leafs_Dynamic(pV->m_lod);
			}
			else 
			{
				if (phase != PHASE_SMAP)
				{
					pV->CalculateBones(true);
					pV->CalculateWallmarks();		//. bug?
				}

				for (dxRender_Visual* pChildVisual : pV->children)
				{
					pChildVisual->vis.obj_data = pV->getVisData().obj_data;
					add_leafs_Dynamic(pChildVisual);
				}
			}
		}
		return;
	default:
		{
			// General type of visual
			// Calculate distance to it's center
			Fvector							Tpos;
			val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
			r_dsgraph_insert_dynamic		(pVisual,Tpos);
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr float O_S_L1_S_ULT = 80.f;
constexpr float O_S_L1_D_ULT = 40.f;
constexpr float O_S_L2_S_ULT = 600.f;
constexpr float O_S_L2_D_ULT = 100.f;
constexpr float O_S_L3_S_ULT = 2500.f;
constexpr float O_S_L3_D_ULT = 120.f;
constexpr float O_S_L4_S_ULT = 5000.f;
constexpr float O_S_L4_D_ULT = 140.f;
constexpr float O_S_L5_S_ULT = 20000.f;
constexpr float O_S_L5_D_ULT = 200.f;


constexpr float O_S_L1_S_MED = 60.f;
constexpr float O_S_L1_D_MED = 45.f;
constexpr float O_S_L2_S_MED = 400.f;
constexpr float O_S_L2_D_MED = 100.f;
constexpr float O_S_L3_S_MED = 2000.f;
constexpr float O_S_L3_D_MED = 160.f;
constexpr float O_S_L4_S_MED = 3800.f;
constexpr float O_S_L4_D_MED = 200.f;
constexpr float O_S_L5_S_MED = 13000.f;
constexpr float O_S_L5_D_MED = 300.f;

// Cut off Static geometry depending of size of geometry and distance to camera and current geometry optimization settings
IC bool IsValuableToRender(dxRender_Visual* pVisual, bool sm)
{
	auto GetDistFromCamera = [](const Fvector & from_position)
	{
		float distance = Device.vCameraPosition.distance_to(from_position);
		float fov_K = 70.f / Device.fFOV;
		float adjusted_distane = distance / fov_K;

		return adjusted_distane;
	};

	if (sm && ps_r_smapsize < 4096)
	{
		float sphere_volume = pVisual->getVisData().sphere.volume();

		float adjusted_distane = GetDistFromCamera(pVisual->vis.sphere.P);

		if (sm && ps_r_smapsize < 4096) // Highest cut off for shadow map
		{
			if (sphere_volume < 50000.f && adjusted_distane > 160) // don't need geometry behind the farest sun shadow cascade
				return false;

			if ((sphere_volume < O_S_L1_S_ULT) && adjusted_distane > O_S_L1_D_ULT)
				return false;
			else if ((sphere_volume < O_S_L2_S_ULT) && adjusted_distane > O_S_L2_D_ULT)
				return false;
			else if ((sphere_volume < O_S_L3_S_ULT) && adjusted_distane > O_S_L3_D_ULT)
				return false;
			else if ((sphere_volume < O_S_L4_S_ULT) && adjusted_distane > O_S_L4_D_ULT)
				return false;
			else if ((sphere_volume < O_S_L5_S_ULT) && adjusted_distane > O_S_L5_D_ULT)
				return false;
			else
				return true;
		}
		else
		{
			if ((sphere_volume < O_S_L1_S_MED) && adjusted_distane > O_S_L1_D_MED)
				return false;
			else if ((sphere_volume < O_S_L2_S_MED) && adjusted_distane > O_S_L2_D_MED)
				return false;
			else if ((sphere_volume < O_S_L3_S_MED) && adjusted_distane > O_S_L3_D_MED)
				return false;
			else if ((sphere_volume < O_S_L4_S_MED) && adjusted_distane > O_S_L4_D_MED)
				return false;
			else if ((sphere_volume < O_S_L5_S_MED) && adjusted_distane > O_S_L5_D_MED)
				return false;
			else
				return true;
		}
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Static(dxRender_Visual* pVisual)
{
	if (!IsValuableToRender(pVisual, phase == PHASE_SMAP))
		return;

	if (!HOM.visible(pVisual->vis))		return;

	// Visual is 100% visible - simply add it
	xr_vector<dxRender_Visual*>::iterator I, E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type)
	{
	case MT_PARTICLE_GROUP:
	{
		if (phase == PHASE_SMAP) return;
		// Add all children, doesn't perform any tests
		PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
		for (PS::CParticleGroup::SItem& refI : pG->items)
		{
			if (refI._effect)		add_leafs_Dynamic(refI._effect);
			for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_related.begin(); pit != refI._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
			for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_free.begin(); pit != refI._children_free.end(); pit++)	add_leafs_Dynamic(*pit);
		}
	}
	return;
	case MT_HIERRARHY:
	{
		// Add all children, doesn't perform any tests
		FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
		I = pV->children.begin();
		E = pV->children.end();

		for (; I != E; I++)
		{
			(*I)->vis.obj_data = pV->getVisData().obj_data;
			add_leafs_Static(*I);
		}
	}
	return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
	{
		// Add all children, doesn't perform any tests
		CKinematics* pV = (CKinematics*)pVisual;
		if (phase != PHASE_SMAP)
			pV->CalculateBones(true);

		I = pV->children.begin();
		E = pV->children.end();

		for (; I != E; I++)
		{
			(*I)->vis.obj_data = pV->getVisData().obj_data;
			add_leafs_Static(*I);
		}
	}
	return;
	case MT_LOD:
	{
		FLOD* pV = (FLOD*)pVisual;
		float		D;
		float		ssa = CalcSSA(D, pV->vis.sphere.P, pV);
		ssa *= pV->lod_factor;
		if (ssa < r_ssaLOD_A)
		{
			if (ssa < r_ssaDISCARD)	return;
			mapLOD.emplace_back(std::make_pair(D, _LodItem({ ssa, pVisual })));
		}

		if (ssa > r_ssaLOD_B || phase == PHASE_SMAP)
		{
			// Add all children, doesn't perform any tests
			I = pV->children.begin();
			E = pV->children.end();
			for (; I != E; I++)
			{
				(*I)->vis.obj_data = pV->getVisData().obj_data;
				add_leafs_Static(*I);
			}
		}
	}
	return;
	case MT_TREE_PM:
	case MT_TREE_ST:
	{
		// General type of visual
		r_dsgraph_insert_static(pVisual);
	}
	return;
	default:
	{
		// General type of visual
		r_dsgraph_insert_static(pVisual);
	}
	return;
	}
}

BOOL CRender::add_Dynamic(dxRender_Visual *pVisual, u32 planes)
{
	if (!IsValuableToRenderDyn(pVisual, *val_pTransform, phase == PHASE_SMAP))
		return FALSE;

	// Check frustum visibility and calculate distance to visual's center
	Fvector		Tpos;	// transformed position
	EFC_Visible	VIS;

	val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
	VIS = View->testSphere			(Tpos, pVisual->vis.sphere.R,planes);
	if (fcvNone==VIS) return false	;

	// If we get here visual is visible or partially visible
	xr_vector<dxRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) 
	{
	case MT_PARTICLE_GROUP:
		{
		if (phase == PHASE_SMAP) return true;
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItem& refI : pG->items)
			{
				if (fcvPartial==VIS) 
				{
					if (refI._effect)		add_Dynamic				(refI._effect,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_related.begin();	pit!=refI._children_related.end(); pit++)	add_Dynamic(*pit,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_free.begin();		pit!=refI._children_free.end();	pit++)	add_Dynamic(*pit,planes);
				} else 
				{
					if (refI._effect)		add_leafs_Dynamic		(refI._effect);
					for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_related.begin();	pit!=refI._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
					for (xr_vector<dxRender_Visual*>::iterator pit = refI._children_free.begin();		pit!=refI._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
				}
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) 
			{
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV			= (CKinematics*)pVisual;
			BOOL	_use_lod			= false	;
			if (pV->m_lod)
			{
				Fvector TPos;
				float D;
				val_pTransform->transform_tiny(TPos, pV->vis.sphere.P);

				// assume dynamics never consume full sphere
				float ssa = CalcSSA(D, TPos, pV->vis.sphere.R / 2.f);	
				if (ssa < r_ssaLOD_A)	_use_lod = true;
			}
			if (_use_lod)
			{
				add_leafs_Dynamic			(pV->m_lod)		;
			} else 
			{
				if (phase != PHASE_SMAP)
				{
					pV->CalculateBones(true);
					pV->CalculateWallmarks();		//. bug?
				}

				I = pV->children.begin		();
				E = pV->children.end		();
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			r_dsgraph_insert_dynamic(pVisual,Tpos);
		}
		break;
	}
	return true;
}

void CRender::add_Static(dxRender_Visual *pVisual, u32 planes)
{
	if (!IsValuableToRender(pVisual, phase == PHASE_SMAP))
		return;

	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
	vis_data&	vis			= pVisual->vis;
	VIS = View->testSAABB	(vis.sphere.P, vis.sphere.R, vis.box.data(), planes);
	if (VIS == fcvNone)
	{
		return;
	}

	if (!HOM.visible(vis))
	{
		return;
	}

	// If we get here visual is visible or partially visible
	switch (pVisual->Type) 
	{
	case MT_PARTICLE_GROUP:
		{
		if (phase == PHASE_SMAP) return;
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItem& I : pG->items)
			{
				if (fcvPartial==VIS) 
				{
					if (I._effect) add_Dynamic (I._effect,planes);

					for (dxRender_Visual* childRelated : I._children_related)
					{
						add_Dynamic(childRelated, planes);
					}

					for (dxRender_Visual* childFree : I._children_free)
					{
						add_Dynamic(childFree, planes);
					}
				} 
				else
				{
					if (I._effect) add_leafs_Dynamic (I._effect);

					for (dxRender_Visual* childRelated : I._children_related)
					{
						add_leafs_Dynamic(childRelated);
					}

					for (dxRender_Visual* childFree : I._children_free)
					{
						add_leafs_Dynamic(childFree);
					}
				}
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			if (VIS == fcvPartial) 
			{
				for (dxRender_Visual* childRenderable : pV->children)
				{
					add_Static(childRenderable, planes);
				}
			} 
			else 
			{
				for (dxRender_Visual* childRenderable : pV->children)
				{
					add_leafs_Static(childRenderable);
				}
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;

			if (phase != PHASE_SMAP)
				pV->CalculateBones(true);

			if (VIS == fcvPartial)
			{
				for (dxRender_Visual* childRenderable : pV->children)
				{
					add_Static(childRenderable, planes);
				}
			} 
			else 
			{
				for (dxRender_Visual* childRenderable : pV->children)
				{
					add_leafs_Static(childRenderable);
				}
			}
		}
		break;
	case MT_LOD:
		{
			FLOD* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			ssa	*= pV->lod_factor;
			if (ssa < r_ssaLOD_A)	
			{
				if (ssa<r_ssaDISCARD)	return;
				mapLOD.emplace_back(std::make_pair(D, _LodItem({ ssa, pVisual })));
			}

			if (ssa > r_ssaLOD_B || phase == PHASE_SMAP)
			{
				// Add all children, perform tests
				for (dxRender_Visual* childRenderable : pV->children)
				{
					add_leafs_Static(childRenderable);
				}
			}
		}
		break;
	case MT_TREE_ST:
	case MT_TREE_PM:
		{
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	default:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		break;
	}
}