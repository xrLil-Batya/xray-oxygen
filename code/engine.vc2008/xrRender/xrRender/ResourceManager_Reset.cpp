#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#ifndef _EDITOR
#include	"../../xrEngine/Render.h"
#else
	#include "../../Include/xrAPI/xrAPI.h"
#endif

void CResourceManager::reset_begin()
{
	// destroy everything, renderer may use
	::Render->reset_begin();

	for (SState* theState : v_states)
	{
		_RELEASE(theState->state);
	}

	// destroy RTs
	for (auto &rt_it : m_rtargets)
		rt_it.second->reset_begin();

	// destroy DStreams
	RCache.old_QuadIB = RCache.QuadIB;
	HW.stats_manager.decrement_stats_ib(RCache.QuadIB);
	_RELEASE(RCache.QuadIB);

	RCache.Index.reset_begin();
	RCache.Vertex.reset_begin();
}

void CResourceManager::reset_end				()
{
	// create RDStreams
	RCache.Vertex.reset_end		();
	RCache.Index.reset_end		();
	RCache.CreateQuadIB();

	// remark geom's which point to dynamic VB/IB
	for (SGeometry* pGeometry : v_geoms)
	{
		if (pGeometry->vb == RCache.Vertex.old_pVB)
			pGeometry->vb = RCache.Vertex.Buffer();

		// Here we may recover the buffer using one of 
		// RCache's index buffers.
		// Do not remove else.
		if (pGeometry->ib == RCache.Index.old_pIB)
		{
			pGeometry->ib = RCache.Index.Buffer();
		}
		else if (pGeometry->ib == RCache.old_QuadIB)
		{
			pGeometry->ib = RCache.QuadIB;
		}
	}

	// create RTs in the same order as them was first created
	{
		// RT
#pragma todo("container is created in stack!")
		xr_vector<CRT*>		rt;
		for (auto rt_it = m_rtargets.begin(); rt_it != m_rtargets.end(); rt_it++)
		{
			rt.push_back(rt_it->second);
		}
		std::sort(rt.begin(),rt.end(), [](const CRT* A, const CRT* B) { return A->_order < B->_order; });
		for (u32 _it=0; _it<rt.size(); _it++)	rt[_it]->reset_end	();
	}

	// create state-blocks
	{
		for (u32 _it=0; _it<v_states.size(); _it++)
			v_states[_it]->state = ID3DState::Create(v_states[_it]->state_code);
	}

	// create everything, renderer may use
	::Render->reset_end		();
//	Dump					(true);
}

template<class C>	void mdump(C c)
{
	if (c.emprty()) return;

	for (C::iterator I = c.begin(); I != c.end(); I++)
	{
		if (I->second && !I->second->cName)
		{
			Msg("* DUMP: %3d: %s", I->second->dwReference, I->second->cName.c_str());
		}
	}
}

CResourceManager::~CResourceManager		()
{
	DestroyNecessaryTextures	();
//	Dump						(false);
}

void CResourceManager::Dump(bool bBrief)
{
//	Msg		("* RM_Dump: textures  : %d",		m_textures.size());		if(!bBrief) mdump(m_textures);
//	Msg		("* RM_Dump: rtargets  : %d",		m_rtargets.size());		if(!bBrief) mdump(m_rtargets);
////	DX10 cut 	Msg		("* RM_Dump: rtargetsc : %d",		m_rtargets_c.size());	if(!bBrief) mdump(m_rtargets_c);
//	Msg		("* RM_Dump: vs        : %d",		m_vs.size());			if(!bBrief) mdump(m_vs);
//	Msg		("* RM_Dump: ps        : %d",		m_ps.size());			if(!bBrief) mdump(m_ps);
//	Msg		("* RM_Dump: dcl       : %d",		v_declarations.size());
//	Msg		("* RM_Dump: states    : %d",		v_states.size());
//	Msg		("* RM_Dump: tex_list  : %d",		lst_textures.size());
//	Msg		("* RM_Dump: matrices  : %d",		lst_matrices.size());
//	Msg		("* RM_Dump: lst_constants: %d",	lst_constants.size());
//	Msg		("* RM_Dump: v_passes  : %d",		v_passes.size());
//	Msg		("* RM_Dump: v_elements: %d",		v_elements.size());
//	Msg		("* RM_Dump: v_shaders : %d",		v_shaders.size());
}
