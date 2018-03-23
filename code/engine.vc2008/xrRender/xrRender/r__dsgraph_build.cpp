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

void R_dsgraph_structure::r_dsgraph_insert_dynamic	(dxRender_Visual *pVisual, Fvector& Center)
{
	CRender&	RI			=	RImplementation;

	if (pVisual->vis.marker	==	RI.marker)	return	;
	pVisual->vis.marker		=	RI.marker			;

	float distSQ			;
	float SSA				=	CalcSSA		(distSQ,Center,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Distortive geometry should be marked and R2 special-cases it
	// a) Allow to optimize RT order
	// b) Should be rendered to special distort buffer in another pass
	VERIFY						(pVisual->shader._get());
	ShaderElement*		sh_d	= &*pVisual->shader->E[4];
	if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority/2]) 
	{
		_MatrixItemS temp;
		temp.ssa		= SSA;
		temp.pObject	= RI.val_pObject;
		temp.pVisual	= pVisual;
		temp.Matrix		= *RI.val_pTransform;
		temp.se			= sh_d;
		mapDistort.emplace_back(std::make_pair(distSQ, temp));
	}

	// Select shader
	ShaderElement*	sh		=	RImplementation.rimp_select_sh_dynamic	(pVisual,distSQ);
	if (0==sh)								return;
	if (!pmask[sh->flags.iPriority/2])		return;

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

			mapSorted.emplace_back(std::make_pair(distSQ, temp));
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
				_MatrixItemS temp;
				temp.ssa = SSA;
				temp.pObject = RI.val_pObject;
				temp.pVisual = pVisual;
				temp.Matrix = *RI.val_pTransform;
				temp.se = &*pVisual->shader->E[4];		// 4=L_special
				mapHUDEmissive.emplace_back(std::make_pair(distSQ, temp));
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
		SPass &pass = *sh->passes[iPass];
		auto &map = mapMatrixPasses[sh->flags.iPriority / 2][iPass];


#ifdef USE_RESOURCE_DEBUGGER
#if defined(USE_DX10) || defined(USE_DX11)
		mapMatrixVS::value_type&			Nvs = map.insert(pass.vs);
		mapMatrixGS::value_type&			Ngs = Nvs->val.insert(pass.gs);
		mapMatrixPS::value_type&			Nps = Ngs->val.insert(pass.ps);
#else	//	USE_DX10
		mapMatrixVS::value_type&			Nvs = map.insert(pass.vs);
		mapMatrixPS::value_type&			Nps = Nvs->val.insert(pass.ps);
#endif	//	USE_DX10
#else
#if defined(USE_DX10) || defined(USE_DX11)
		auto &Nvs = map[&*pass.vs];
		auto &Ngs = Nvs[pass.gs->gs];
		auto &Nps = Ngs[pass.ps->ps];
#else
		auto &Nvs = map[pass.vs->vs];
		auto &Nps = Nvs[pass.ps->ps];
#endif
#endif

#ifdef USE_DX11
		Nps.hs = pass.hs->sh;
		Nps.ds = pass.ds->sh;

		auto &Ncs = Nps.mapCS[pass.constants._get()];
#else
		R_dsgraph::mapMatrixStates &Ncs = Nps[pass.constants._get()];
#endif
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
#ifdef USE_DX11
					if (SSA > Nps.mapCS.ssa)
					{
						Nps.mapCS.ssa = SSA;
#else
					if (SSA > Nps.ssa)
					{
						Nps.ssa = SSA;
#endif
#if defined(USE_DX10) || defined(USE_DX11)
						if (SSA > Ngs.ssa)
						{
							Ngs.ssa = SSA;
#endif
							if (SSA > Nvs.ssa)
							{
								Nvs.ssa = SSA;
							}
#if defined(USE_DX10) || defined(USE_DX11)
						}
#endif
					}
				}
			}
		}
	}

	if (val_recorder)
	{
		val_recorder->push_back(pVisual->vis.box);
	}
}

void R_dsgraph_structure::r_dsgraph_insert_static	(dxRender_Visual *pVisual)
{
	CRender&	RI				=	RImplementation;

	if (pVisual->vis.marker		==	RI.marker)	return	;
	pVisual->vis.marker			=	RI.marker			;

	float distSQ;
	float SSA					=	CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);
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
	if (sh->flags.bStrictB2F) {
		_MatrixItemS temp;
		temp.pObject = NULL;
		temp.pVisual = pVisual;
		temp.Matrix = Fidentity;
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
		SPass&						pass	= *sh->passes[iPass];
		mapNormal_T&				map		= mapNormalPasses[sh->flags.iPriority/2][iPass];

#ifdef USE_RESOURCE_DEBUGGER
#	if defined(USE_DX10) || defined(USE_DX11)
		mapNormalVS::value_type&			Nvs		= map.insert		(pass.vs);
		mapNormalGS::value_type&			Ngs		= Nvs->val.insert	(pass.gs);
		mapNormalPS::value_type&			Nps		= Ngs->val.insert	(pass.ps);
#	else	//	USE_DX10
		mapNormalVS::value_type&			Nvs		= map.insert		(pass.vs);
		mapNormalPS::value_type&			Nps		= Nvs->val.insert	(pass.ps);
#	endif	//	USE_DX10
#else // USE_RESOURCE_DEBUGGER

#if defined(USE_DX10) || defined(USE_DX11)
		auto &Nvs = map[&*pass.vs];
		auto &Ngs = Nvs[pass.gs->gs];
		auto &Nps = Ngs[pass.ps->ps];
#else
		auto &Nvs = map[pass.vs->vs];
		auto &Nps = Nvs[pass.ps->ps];
#endif
#endif

#ifdef USE_DX11
		Nps.hs = pass.hs->sh;
		Nps.ds = pass.ds->sh;

		auto &Ncs = Nps.mapCS[pass.constants._get()];
#else
		auto &Ncs = Nps[pass.constants._get()];
#endif
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
#ifdef USE_DX11
					if (SSA > Nps.mapCS.ssa)
					{
						Nps.mapCS.ssa = SSA;
#else
					if (SSA > Nps.ssa)
					{
						Nps.ssa = SSA;
#endif
#if defined(USE_DX10) || defined(USE_DX11)
						if (SSA > Ngs.ssa)
						{
							Ngs.ssa = SSA;
#endif
							if (SSA > Nvs.ssa)
							{
								Nvs.ssa = SSA;
							}
#if defined(USE_DX10) || defined(USE_DX11)
						}
#endif
					}
				}
			}
		}
	}

	if (val_recorder)
	{
		val_recorder->push_back(pVisual->vis.box);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Dynamic	(dxRender_Visual *pVisual)
{
	if (0==pVisual)				return;

	// Visual is 100% visible - simply add it
	xr_vector<dxRender_Visual*>::iterator I,E;	// it may be useful for 'hierrarhy' visual

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG	= (PS::CParticleGroup*)pVisual;
			for (auto i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++)	{
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (I._effect)		add_leafs_Dynamic		(I._effect);
				for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
				for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
			}
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I != E; I++)
			{
				(*I)->vis.obj_data = pV->getVisData().obj_data;
				add_leafs_Dynamic(*I);
			}
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV			= (CKinematics*)pVisual;
			BOOL	_use_lod			= FALSE	;
			if (pV->m_lod)				
			{
				Fvector							Tpos;	float		D;
				val_pTransform->transform_tiny	(Tpos, pV->vis.sphere.P);
				float		ssa		=	CalcSSA	(D,Tpos,pV->vis.sphere.R/2.f);	// assume dynamics never consume full sphere
				if (ssa<r_ssaLOD_A)	_use_lod	= TRUE;
			}
			if (_use_lod)				
			{
				add_leafs_Dynamic			(pV->m_lod)		;
			} else {
				pV->CalculateBones			(TRUE);
				pV->CalculateWallmarks		();		//. bug?
				I = pV->children.begin		();
				E = pV->children.end		();

				for (; I != E; I++)
				{
					(*I)->vis.obj_data = pV->getVisData().obj_data;
					add_leafs_Dynamic(*I);
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

void CRender::add_leafs_Static(dxRender_Visual *pVisual)
{
	if (!HOM.visible(pVisual->vis))		return;

	// Visual is 100% visible - simply add it
	xr_vector<dxRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (auto i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++){
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (I._effect)		add_leafs_Dynamic		(I._effect);
				for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
				for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
			}
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV	= (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();

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
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->CalculateBones		(TRUE);
			I = pV->children.begin	();
			E = pV->children.end	();

			for (; I != E; I++)
			{
				(*I)->vis.obj_data = pV->getVisData().obj_data;
				add_leafs_Static(*I);
			}
		}
		return;
	case MT_LOD:
		{
			FLOD		* pV	=		(FLOD*) pVisual;
			float		D;
			float		ssa		=		CalcSSA(D,pV->vis.sphere.P,pV);
			ssa					*=		pV->lod_factor;
			if (ssa<r_ssaLOD_A)
			{
				if (ssa<r_ssaDISCARD)	return;
				mapLOD.emplace_back(std::make_pair(D, _LodItem({ ssa, pVisual })));
			}

			if (ssa>r_ssaLOD_B || phase==PHASE_SMAP)
			{
				// Add all children, doesn't perform any tests
				I = pV->children.begin	();
				E = pV->children.end	();
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
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	default:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRender::add_Dynamic(dxRender_Visual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	Fvector		Tpos;	// transformed position
	EFC_Visible	VIS;

	val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
	VIS = View->testSphere			(Tpos, pVisual->vis.sphere.R,planes);
	if (fcvNone==VIS) return FALSE	;

	// If we get here visual is visible or partially visible
	xr_vector<dxRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (auto i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++)
			{
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (fcvPartial==VIS) 
				{
					if (I._effect)		add_Dynamic				(I._effect,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_Dynamic(*pit,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_Dynamic(*pit,planes);
				} else 
				{
					if (I._effect)		add_leafs_Dynamic		(I._effect);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
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
			BOOL	_use_lod			= FALSE	;
			if (pV->m_lod)				
			{
				Fvector							Tpos;	float		D;
				val_pTransform->transform_tiny	(Tpos, pV->vis.sphere.P);
				float		ssa		=	CalcSSA	(D,Tpos,pV->vis.sphere.R/2.f);	// assume dynamics never consume full sphere
				if (ssa<r_ssaLOD_A)	_use_lod	= TRUE		;
			}
			if (_use_lod)
			{
				add_leafs_Dynamic			(pV->m_lod)		;
			} else 
			{
				pV->CalculateBones			(TRUE);
				pV->CalculateWallmarks		();		//. bug?
				I = pV->children.begin		();
				E = pV->children.end		();
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
			/*
			I = pV->children.begin		();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
			*/
		}
		break;
	default:
		{
			// General type of visual
			r_dsgraph_insert_dynamic(pVisual,Tpos);
		}
		break;
	}
	return TRUE;
}

void CRender::add_Static(dxRender_Visual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
	vis_data&	vis			= pVisual->vis;
	VIS = View->testSAABB	(vis.sphere.P,vis.sphere.R,vis.box.data(),planes);
	if (fcvNone==VIS)		
		return;

	if (!HOM.visible(vis))	
		return;

	// If we get here visual is visible or partially visible
	xr_vector<dxRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (auto i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++){
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (fcvPartial==VIS) {
					if (I._effect)		add_Dynamic				(I._effect,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_Dynamic(*pit,planes);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_Dynamic(*pit,planes);
				} else {
					if (I._effect)		add_leafs_Dynamic		(I._effect);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
					for (xr_vector<dxRender_Visual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
				}
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->CalculateBones		(TRUE);
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			ssa					*= pV->lod_factor;
			if (ssa<r_ssaLOD_A)	
			{
				if (ssa<r_ssaDISCARD)	return;
				mapLOD.emplace_back(std::make_pair(D, _LodItem({ ssa, pVisual })));
			}

			if (ssa>r_ssaLOD_B || phase==PHASE_SMAP)
			{
				// Add all children, perform tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_TREE_ST:
	case MT_TREE_PM:
		{
			// General type of visual
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
