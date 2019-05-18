#include "stdafx.h"
#pragma hdrstop

#include "ModelPool.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/fmesh.h"
#include "fhierrarhyvisual.h"
#include "SkeletonAnimated.h"
#include "fvisual.h"
#include "fprogressive.h"
#include "fskinned.h"
#include "flod.h"
#include "ftreevisual.h"
#include "ParticleGroup.h"
#include "ParticleEffect.h"

dxRender_Visual* CModelPool::Instance_Create(u32 type)
{
	dxRender_Visual *pVisualDX = nullptr;

	// Check types
	switch (type)
	{
	case MT_NORMAL:					pVisualDX = new Fvisual(); break; // our base visual
	case MT_HIERRARHY:				pVisualDX = new FHierrarhyVisual(); break;
	case MT_PROGRESSIVE:			pVisualDX = new FProgressive(); break;  // dynamic-resolution visual
	case MT_SKELETON_ANIM:			pVisualDX = new CKinematicsAnimated(); break;
	case MT_SKELETON_RIGID:			pVisualDX = new CKinematics(); break;
	case MT_SKELETON_GEOMDEF_PM:	pVisualDX = new CSkeletonX_PM(); break;
	case MT_SKELETON_GEOMDEF_ST:	pVisualDX = new CSkeletonX_ST(); break;
	case MT_PARTICLE_EFFECT:		pVisualDX = new PS::CParticleEffect(); break;
	case MT_PARTICLE_GROUP:			pVisualDX = new PS::CParticleGroup(); break;
	case MT_LOD:					pVisualDX = new FLOD(); break;
	case MT_TREE_ST:				pVisualDX = new FTreeVisual_ST(); break;
	case MT_TREE_PM:				pVisualDX = new FTreeVisual_PM(); break;
	default:						FATAL("Unknown visual type"); break;
	}

	pVisualDX->Type = type;
	return pVisualDX;
}

dxRender_Visual* CModelPool::Instance_Duplicate(dxRender_Visual* pVisualDX)
{
	R_ASSERT(pVisualDX);
	dxRender_Visual* N = Instance_Create(pVisualDX->Type);
	N->Copy(pVisualDX);
	N->Spawn();

	// inc ref counter
	for (auto &refModel : Models)
	{
		if (refModel.model == pVisualDX)
		{
			refModel.refs++;
			break;
		}
	}

	return N;
}

dxRender_Visual* CModelPool::TryLoadObject(const char* N)
{
	return nullptr;
}
dxRender_Visual* CModelPool::TryLoadOgf(const char* N)
{
	string_path fn;
	string_path name;

	// Add default ext if no ext at all
	if (!strext(N))
		xr_strconcat(name, N, ".ogf");
	else
		strcpy_s(name, sizeof(name), N);

	// Load data from MESHES or LEVEL
	if (!FS.exist(N)) 
	{
		if (!FS.exist(fn, "$level$", name) && !FS.exist(fn, "$game_meshes$", name))
			return nullptr;
	}
	else strcpy_s(fn, N);

	// Actual loading
	IReader* data = FS.r_open(fn);
	ogf_header H;
	data->r_chunk_safe(OGF_HEADER, &H, sizeof(H));

	dxRender_Visual* pVisualDX = Instance_Create(H.type);
	pVisualDX->Load(N, data, 0);
	FS.r_close(data);

	return pVisualDX;
}

dxRender_Visual* CModelPool::Instance_Load(const char* N, BOOL allow_register)
{
	dxRender_Visual* pVisualDX = TryLoadOgf(N);

	if (!pVisualDX)
		Debug.fatal(DEBUG_INFO, "Can't find model file '%s'.", N);

	g_pGamePersistent->RegisterModel(pVisualDX);

	// Registration
	if (allow_register) 
		Instance_Register(N, pVisualDX);

	return pVisualDX;
}

dxRender_Visual* CModelPool::Instance_Load(LPCSTR name, IReader* data, BOOL allow_register)
{
	ogf_header H;
	data->r_chunk_safe(OGF_HEADER, &H, sizeof(H));
	dxRender_Visual	*pVisualDX = Instance_Create(H.type);
	pVisualDX->Load(name, data, 0);

	// Registration
	if (allow_register) 
		Instance_Register(name, pVisualDX);

	return pVisualDX;
}

void CModelPool::Instance_Register(LPCSTR N, dxRender_Visual* V)
{
	// Registration
	ModelDef M;
	M.name = N;
	M.model = V;
	Models.push_back(M);
}

void CModelPool::Destroy()
{
	// Pool
	Pool.clear			();

	// Registry
	while (!Registry.empty()) 
	{
		REGISTRY_IT it = Registry.begin();
		dxRender_Visual* V = (dxRender_Visual*)it->first;
		DeleteInternal(V, TRUE);
	}

	// Base/Reference
	for (auto &refModel: Models)
	{
		refModel.model->Release();
		xr_delete(refModel.model);
	}
	
	Models.clear();

	// cleanup motions container
	g_pMotionsContainer->clean(false);
}

CModelPool::CModelPool()
{
	bLogging				= TRUE;
    bForceDiscard 			= FALSE;
    bAllowChildrenDuplicate	= TRUE; 
	g_pMotionsContainer		= new motions_container();
}

CModelPool::~CModelPool()
{
	Destroy					();
	xr_delete				(g_pMotionsContainer);
}

dxRender_Visual* CModelPool::Instance_Find(LPCSTR N)
{
	dxRender_Visual*				Model=nullptr;
	xr_vector<ModelDef>::iterator	I;

	for (I=Models.begin(); I!=Models.end(); I++)
		if (I->name[0]&&(0==xr_strcmp(*I->name,N))) 
		{
			Model = I->model;
			break;
		}

	return Model;
}

dxRender_Visual* CModelPool::Create(const char* name, IReader* data)
{
	string_path low_name;
	xr_strcpy(low_name, name);	strlwr(low_name);
	if (strext(low_name))	*strext(low_name) = 0;

	// 0. Search POOL
	POOL_IT	it = Pool.find(low_name);
	if (it != Pool.end())
	{
		// 1. Instance found
		dxRender_Visual* Model = it->second;
		Model->Spawn();

		mtPeref.Enter();
		Pool.erase(it);
		mtPeref.Leave();

		return Model;
	}
	else 
	{
		// 1. Search for already loaded model (reference, base model)
		dxRender_Visual* Base = Instance_Find(low_name);

		mtPeref.Enter();
		if (!Base) 
		{
			// 2. If not found
			bAllowChildrenDuplicate = FALSE;
			if (data)		Base = Instance_Load(low_name, data, TRUE);
			else			Base = Instance_Load(low_name, TRUE);
			bAllowChildrenDuplicate = TRUE;
		}
		// 3. If found - return (cloned) reference
		dxRender_Visual* Model = Instance_Duplicate(Base);

		Registry.insert(std::make_pair(Model, low_name));
		mtPeref.Leave();

		return Model;
	}
}

dxRender_Visual* CModelPool::CreateChild(LPCSTR name, IReader* data)
{
	string256 low_name;		VERIFY	(xr_strlen(name)<256);
	xr_strcpy(low_name,name);	strlwr	(low_name);
	if (strext(low_name))	*strext	(low_name) = 0;

	// 1. Search for already loaded model
	dxRender_Visual* Base	= Instance_Find(low_name);

	if(0==Base)
	{
		if (data)		Base = Instance_Load	(low_name,data,FALSE);
		else			Base = Instance_Load	(low_name,FALSE);
	}

    dxRender_Visual* Model	= bAllowChildrenDuplicate?Instance_Duplicate(Base):Base;
    return					Model;
}

extern  BOOL ENGINE_API g_bRendering; 
void	CModelPool::DeleteInternal	(dxRender_Visual* &V, BOOL bDiscard)
{
	VERIFY					(!g_bRendering);
    if (!V)					return;
	V->Depart				();
	if (bDiscard||bForceDiscard){
    	Discard	(V, TRUE); 
	}else{
		//
		REGISTRY_IT	it		= Registry.find	(V);
		if (it!=Registry.end())
		{
			// Registry entry found - move it to pool
			Pool.insert			(std::make_pair(it->second,V));
		} else {
			// Registry entry not-found - just special type of visual / particles / etc.
			xr_delete			(V);
		}
	}
	V	=	NULL;
}

void CModelPool::Delete(dxRender_Visual* &V, BOOL bDiscard)
{
	if (!V) return;

	if (g_bRendering) 
	{
		VERIFY(!bDiscard);
		ModelsToDelete.push_back(V);
	}
	else 
	{
		DeleteInternal(V, bDiscard);
	}

	V = nullptr;
}

void CModelPool::DeleteQueue()
{
	for (dxRender_Visual* pVisualDX: ModelsToDelete)
		DeleteInternal(pVisualDX);
	ModelsToDelete.clear();
}

void CModelPool::Discard(dxRender_Visual* &V, BOOL b_complete)
{
	REGISTRY_IT	it = Registry.find	(V);
	// Pool - OK
	if (it!=Registry.end())
	{
		// Base
		const shared_str& name = it->second;
		u32 IterCont = 0;
		for (auto &refModel : Models)
		{
			if (refModel.name == name)
			{
				if (b_complete || strchr(name.c_str(), '#'))
				{
					VERIFY(refModel.refs > 0);
					refModel.refs--;
					if (!refModel.refs)
					{
						bForceDiscard = TRUE;
						refModel.model->Release();
						xr_delete(refModel.model);
						Models.erase(Models.begin() + IterCont);
						bForceDiscard = FALSE;
					}
				}
				else if (refModel.refs > 0)
				{
					refModel.refs--;
				}
				break;
			}
			IterCont++;
		}

		// Registry
		xr_delete		(V);	
		Registry.erase	(it);
	} else {
		// Registry entry not-found - just special type of visual / particles / etc.
		xr_delete		(V);
	}
	V	=	nullptr;
}

void CModelPool::Prefetch()
{
	Logging(FALSE);
	// prefetch visuals
	string256 section;
	xr_strconcat( section, "prefetch_visuals_", g_pGamePersistent->m_game_params.m_game_type);
	CInifile::Sect& sect = pSettings->r_section(section);

    for (const CInifile::Item &it : sect.Data)
    {
        dxRender_Visual* pVis = Create(it.first.c_str());
    }

	Logging(TRUE);
}

void CModelPool::ClearPool( BOOL b_complete)
{
	POOL_IT	_I			=	Pool.begin();
	POOL_IT	_E			=	Pool.end();
	for (;_I!=_E;_I++)	{
		Discard	(_I->second, b_complete)	;
	}
	Pool.clear			();
}

dxRender_Visual* CModelPool::CreatePE	(PS::CPEDef* source)
{
	PS::CParticleEffect* V	= (PS::CParticleEffect*)Instance_Create(MT_PARTICLE_EFFECT);
	V->Compile		(source);
	return V;
}

dxRender_Visual* CModelPool::CreatePG	(PS::CPGDef* source)
{
	PS::CParticleGroup* V	= (PS::CParticleGroup*)Instance_Create(MT_PARTICLE_GROUP);
	V->Compile		(source);
	return V;
}

void CModelPool::dump()
{
	Log	("--- model pool --- begin:");
	u32 sz					= 0;
	u32 k					= 0;
	for (xr_vector<ModelDef>::iterator I=Models.begin(); I!=Models.end(); I++) {
		CKinematics* K		= PCKinematics(I->model);
		if (K){
			u32 cur			= K->mem_usage	(false);
			sz				+= cur;
			Msg("#%3d: [%3d/%5d Kb] - %s",k++,I->refs,cur/1024,I->name.c_str());
		}
	}
	Msg ("--- models: %d, mem usage: %d Kb ",k,sz/1024);
	sz						= 0;
	k						= 0;
	int free_cnt			= 0;
	for (REGISTRY_IT it=Registry.begin(); it!=Registry.end(); it++)
	{
		CKinematics* K		= PCKinematics((dxRender_Visual*)it->first);
		VERIFY				(K);
		if (K){
			u32 cur			= K->mem_usage	(true);
			sz				+= cur;
			bool b_free		= (Pool.find(it->second)!=Pool.end() );
			if(b_free)		++free_cnt;
			Msg("#%3d: [%s] [%5d Kb] - %s",k++, (b_free)?"free":"used", cur/1024,it->second.c_str());
		}
	}
	Msg ("--- instances: %d, free %d, mem usage: %d Kb ",k, free_cnt, sz/1024);
	Log	("--- model pool --- end.");
}

void CModelPool::memory_stats		( u32& vb_mem_video, u32& vb_mem_system, u32& ib_mem_video, u32& ib_mem_system )
{
	vb_mem_video = 0;
	vb_mem_system = 0;
	ib_mem_video = 0;
	ib_mem_system = 0;

	xr_vector<ModelDef>::iterator		it = Models.begin();
	xr_vector<ModelDef>::const_iterator	en = Models.end();

	for(; it != en; ++it )
	{
		dxRender_Visual* ptr = it->model;
		Fvisual* vis_ptr = dynamic_cast<Fvisual*> (ptr);

		if( vis_ptr == nullptr)
			continue;

#ifndef USE_DX11
		D3DINDEXBUFFER_DESC IB_desc;
		D3DVERTEXBUFFER_DESC VB_desc;

		vis_ptr->m_fast->p_rm_Indices->GetDesc( &IB_desc );

		D3DPOOL         IB_Pool = IB_desc.Pool;
		unsigned int    IB_Size = IB_desc.Size;

		if( IB_Pool == D3DPOOL_DEFAULT ||
			IB_Pool == D3DPOOL_MANAGED )
			ib_mem_video += IB_Size;

		if( IB_Pool == D3DPOOL_MANAGED ||
			IB_Pool == D3DPOOL_SCRATCH )
			ib_mem_system += IB_Size;

		vis_ptr->m_fast->p_rm_Vertices->GetDesc( &VB_desc );

		D3DPOOL         VB_Pool = VB_desc.Pool;
		unsigned int    VB_Size = VB_desc.Size;

		if( VB_Pool == D3DPOOL_DEFAULT ||
			VB_Pool == D3DPOOL_MANAGED)
			vb_mem_video += VB_Size;

		if( VB_desc.Pool == D3DPOOL_MANAGED ||
			VB_desc.Pool == D3DPOOL_SCRATCH )
			vb_mem_system += VB_Size;

#else
		D3D_BUFFER_DESC IB_desc;
		D3D_BUFFER_DESC VB_desc;

		vis_ptr->m_fast->p_rm_Indices->GetDesc( &IB_desc );

		ib_mem_video += IB_desc.ByteWidth;
		ib_mem_system += IB_desc.ByteWidth;

		vis_ptr->m_fast->p_rm_Vertices->GetDesc( &VB_desc );

		vb_mem_video += IB_desc.ByteWidth;
		vb_mem_system += IB_desc.ByteWidth;

#endif
	}
} 