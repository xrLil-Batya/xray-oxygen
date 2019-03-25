#include "stdafx.h"
//#include "cl_collector.h"
#include "build.h"
#include "../xrLC_Light/xrMU_Model.h"
#include "../xrLC_Light/xrMU_Model_Reference.h"

#include "../xrLC_Light/xrLC_GlobalData.h"
#include "../../xrcdb/xrcdb.h"
#include "../xrLC_Light/xrface.h"

CDB::MODEL*	RCAST_Model	= 0;

IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void SaveUVM			(LPCSTR fname, xr_vector<b_rc_face>& vm)
{
	IWriter* W			= FS.w_open(fname);
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<vm.size(); v_idx++){
		b_rc_face& rcf	= vm[v_idx];
		xr_sprintf			(tmp,"f %d %d [%3.2f,%3.2f]-[%3.2f,%3.2f]-[%3.2f,%3.2f]",rcf.dwMaterial,rcf.dwMaterialGame,
						rcf.t[0].x,rcf.t[0].y, rcf.t[1].x,rcf.t[1].y, rcf.t[2].x,rcf.t[2].y);
		W->w_string		(tmp);
	}
	FS.w_close	(W);
}

void CBuild::BuildRapid		(BOOL bSaveForOtherCompilers)
{
	float	p_total			= 0;
	float	p_cost			= 1.f/(lc_global_data()->g_faces().size());

	
	lc_global_data()->destroy_rcmodel();
	Logger.Status			("Converting faces...");
	for				(u32 fit=0; fit<lc_global_data()->g_faces().size(); fit++)	lc_global_data()->g_faces()[fit]->flags.bProcessed = false;

	xr_vector<Face*>			adjacent_vec;
	adjacent_vec.reserve		(6*2*3);

	CDB::CollectorPacked	CL	(scene_bb, (u32)lc_global_data()->g_vertices().size(), (u32)lc_global_data()->g_faces().size());

	for (vecFaceIt it=lc_global_data()->g_faces().begin(); it!=lc_global_data()->g_faces().end(); it++)
	{
		Face*	F				= (*it);
		const Shader_xrLC&	SH		= F->Shader();
		if (!SH.flags.bLIGHT_CastShadow)					continue;

		Logger.Progress	(float(it-lc_global_data()->g_faces().begin())/float(lc_global_data()->g_faces().size()));

		// Collect
		adjacent_vec.clear	();
		for (int vit=0; vit<3; ++vit)
		{
			Vertex* V = F->v[vit];
			for (u32 adj=0; adj<V->m_adjacents.size(); adj++)
			{
				adjacent_vec.push_back(V->m_adjacents[adj]);
			}
		}
		std::sort		(adjacent_vec.begin(),adjacent_vec.end());
		adjacent_vec.erase	(std::unique(adjacent_vec.begin(),adjacent_vec.end()),adjacent_vec.end());

		// Unique
		bool bAlready = false;
		for (u32 ait=0; ait<adjacent_vec.size(); ++ait)
		{
			Face* Test = adjacent_vec[ait];
			if (Test==F)					continue;
			if (!Test->flags.bProcessed)	continue;
			if (FaceEqual(*F,*Test))
			{
				bAlready = true;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->flags.bProcessed	= true;
			CL.add_face_D(F->v[0]->P, F->v[1]->P, F->v[2]->P, *((size_t*)&F), F->sm_group);
		}
	}

	// Export references
	if (bSaveForOtherCompilers)		Logger.Phase	("Building rcast-CFORM-mu model...");
	Logger.Status					("Models...");
	for (u32 ref=0; ref<mu_refs().size(); ref++)
		mu_refs()[ref]->export_cform_rcast	(CL);

	// "Building tree..
	Logger.Status					("Building search tree...");
	lc_global_data()->create_rcmodel( CL );

	extern void SaveAsSMF			(LPCSTR fname, CDB::CollectorPacked& CL);
	
	// save source SMF
	string_path				fn;

	bool					keep_temp_files = !!strstr(Core.Params,"-keep_temp_files");
	if (g_params().m_quality!=ebqDraft) {
		if (keep_temp_files)
			SaveAsSMF		(xr_strconcat(fn,pBuild->path,"build_cform_source.smf"),CL);
	}

	// Saving for AI/DO usage
	if (bSaveForOtherCompilers)
	{
		Logger.Status					("Saving...");
		string_path				fn;

		xr_vector<b_rc_face>	rc_faces;
		rc_faces.resize			(CL.getTS());
		// Prepare faces
		for (u32 k=0; k<CL.getTS(); k++){
			CDB::TRI& T			= CL.getT( k );
			base_Face* F		= (base_Face*)(*((void**)&T.dummy));
			b_rc_face& cf		= rc_faces[k];
			cf.dwMaterial		= F->dwMaterial;
			cf.dwMaterialGame	= F->dwMaterialGame;
			Fvector2*	cuv		= F->getTC0	();
			cf.t[0].set			(cuv[0]);
			cf.t[1].set			(cuv[1]);
			cf.t[2].set			(cuv[2]);
		}
		if (g_params().m_quality!=ebqDraft) {
			if (keep_temp_files)
				SaveUVM			(xr_strconcat(fn,pBuild->path,"build_cform_source.uvm"),rc_faces);
		}

		xr_strconcat(fn, pBuild->path, "build.rc_faces");
		IWriter* Faces_FS = FS.w_open(fn);
		Faces_FS->open_chunk(0);
		for (auto &it : rc_faces)
		{
			Faces_FS->w_u16(it.reserved);
			Faces_FS->w_u16(it.dwMaterial);
			Faces_FS->w_u32(it.dwMaterialGame);
			
			Faces_FS->w_fvector2(it.t[0]);
			Faces_FS->w_fvector2(it.t[1]);
			Faces_FS->w_fvector2(it.t[2]);
		}
		
		Faces_FS->close_chunk();
		FS.w_close(Faces_FS);
	}
}