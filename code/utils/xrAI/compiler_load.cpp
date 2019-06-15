#include "stdafx.h"
#include "compiler.h"
#include "levelgamedef.h"
#include "level_graph.h"
#include "AIMapExport.h"
#include <memory>

bool is_thm_missing		= false;
bool is_thm_deprecated	= false;
bool is_tga_missing		= false;

IC	const Fvector vertex_position(const CLevelGraph::CPosition &Psrc, const Fbox &bb, const SAIParams &params)
{
	Fvector				Pdest;
	int	x,z, row_length;
	row_length			= iFloor((bb.max.z - bb.min.z)/params.fPatchSize + EPS_L + 1.5f);
	x					= Psrc.xz() / row_length;
	z					= Psrc.xz() % row_length;
	Pdest.x =			float(x)*params.fPatchSize + bb.min.x;
	Pdest.y =			(float(Psrc.y())/65535)*(bb.max.y-bb.min.y) + bb.min.y;
	Pdest.z =			float(z)*params.fPatchSize + bb.min.z;
	return				(Pdest);
}

struct CNodePositionConverter {
	IC		CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np);
};

IC CNodePositionConverter::CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np)
{
	Fvector		Pdest;
	Pdest.x		= float(Psrc.x)*m_header.size;
	Pdest.y		= (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
	Pdest.z		= float(Psrc.z)*m_header.size;
	CNodePositionCompressor(np,Pdest,m_header);
	np.y		(Psrc.y);
}

//-----------------------------------------------------------------
#ifndef PVS_STUDIO
template <class T>
void transfer(const char *name, xr_vector<T> &dest, IReader& F, u32 chunk)
{
	IReader*	O	= F.open_chunk(chunk);
	u32		count	= O?(O->length()/sizeof(T)):0;
	Logger.clMsg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->pointer(), (T*)O->pointer() + count);
	}
	if (O)		O->close	();
}
#endif

extern u32*		Surface_Load	(char* name, u32& w, u32& h);
extern void		Surface_Init	();

void xrLoad(LPCSTR name, bool draft_mode)
{
	FS.get_path					("$level$")->_set	((LPSTR)name);
	string256					N;
	if (!draft_mode)	{
		// shaders
		string_path				N;
		FS.update_path			(N,"$game_data$","shaders_xrlc.xr");
		g_shaders_xrlc			= xr_new<Shader_xrLC_LIB> ();
		g_shaders_xrlc->Load	(N);

		// Load CFORM
		{
			string_path cform_path, rc_face_path;
			xr_strconcat(cform_path, name, "level.cform");
			IReader* fs = FS.r_open(cform_path);
			R_ASSERT2(fs, "can`t load level.cform");

			hdrCFORM			H;
			
			fs->r				(&H,sizeof(hdrCFORM));
//			R_ASSERT			(CFORM_CURRENT_VERSION==H.version);

			Fvector*	verts	= (Fvector*)fs->pointer();

			CDB::TRI* build_tris = (CDB::TRI*)(verts + H.vertcount);
			Level.build(verts, H.vertcount, build_tris, H.facecount/* , nullptr, nullptr, false */);

			Level.syncronize	();
			Msg("* Level CFORM: %dK",Level.memory()/1024);

			g_rc_faces.resize	(H.facecount);
			/////////////////////////////////////////////////////////////////////
			// New rc_face reader
			xr_strconcat(rc_face_path, name, "build.rc_faces");
			IReader* Face_fs = FS.r_open(rc_face_path);
			R_ASSERT2(Face_fs, "can`t load build.rc_faces");

			Face_fs->open_chunk(0);
			for (auto &it : g_rc_faces)
			{
				it.reserved = Face_fs->r_u16();
				it.dwMaterial = Face_fs->r_u16();
				it.dwMaterialGame = Face_fs->r_u32();
				Face_fs->r_fvector2(it.t[0]);
				Face_fs->r_fvector2(it.t[1]);
				Face_fs->r_fvector2(it.t[2]);
			}
			Face_fs->close();
			/////////////////////////////////////////////////////////////////////
			LevelBB.set			(H.aabb);
			FS.r_close			(fs);
		}

		// Load level data
		{
			xr_strconcat(N,name,"build.prj");
			IReader* fs = FS.r_open (N);
			R_ASSERT2(fs, "There is no file 'build.prj'!"); // Вот этот вылет никогда раньше не мог появится, хоть и был всё время

			// Version
			u32 version;
			fs->r_chunk			(EB_Version,&version);

			R_ASSERT2			(version >= 17, "xrAI don't support a current version. Sorry.");
			R_ASSERT2			(version <= 19, "xrAI don't support a current version. Sorry.");

			// Load level data
			transfer("materials",	g_materials,			*fs,		EB_Materials);
			transfer("shaders_xrlc",g_shader_compile,		*fs,		EB_Shaders_Compile);

			// process textures
			Logger.Status			("Processing textures...");
			{
				Surface_Init		();
				IReader* F = fs->open_chunk	(EB_Textures);
				u32 tex_count		= F->length() / sizeof(help_b_texture);

				for (u32 t=0; t<tex_count; t++)
				{
					Logger.Progress(float(t)/float(tex_count));

					// workaround for ptr size mismatching
					help_b_texture	TEX;
					F->r(&TEX, sizeof(TEX));
					b_BuildTexture	BT;
					std::memcpy(&BT, &TEX, sizeof(TEX) - 4);	// ptr should be copied separately
					BT.pSurface = nullptr;

					// load thumbnail
					string128		&N = BT.name;
					LPSTR			extension = strext(N);
					if (extension)
						*extension	= 0;

					xr_strlwr		(N);

					if (!xr_strcmp(N,"level_lods"))	
					{
						// HACK for merged lod textures
						BT.dwWidth	= 1024;
						BT.dwHeight	= 1024;
						BT.bHasAlpha= TRUE;
					} 
					else 
					{
						xr_strcat(N,".thm");
						IReader* THM = FS.r_open("$game_textures$",N);
						
						if (!THM)
						{
							Logger.clMsg("can't find thm: %s", N);
							is_thm_missing = true;
							continue;
						}
						// version
						u32 version = 0;

						if (!THM->r_chunk(THM_CHUNK_VERSION, &version))
						{
							Logger.clMsg("xrAI don't support a current version %s.thm.", N);
							is_thm_deprecated = true;
						}
						//R_ASSERT3(THM->r_chunk(THM_CHUNK_VERSION,&version), "xrAI don't support a current version %s.thm", N);

						// analyze thumbnail information
						R_ASSERT(THM->find_chunk(THM_CHUNK_TEXTUREPARAM));
						THM->r                  (&BT.THM.fmt,sizeof(STextureParams::ETFormat));
						BT.THM.flags.assign		(THM->r_u32());
						BT.THM.border_color		= THM->r_u32();
						BT.THM.fade_color		= THM->r_u32();
						BT.THM.fade_amount		= THM->r_u32();
						BT.THM.mip_filter		= THM->r_u32();
						BT.THM.width			= THM->r_u32();
						BT.THM.height           = THM->r_u32();

						const bool bLOD = (N[0] == 'l' && N[1] == 'o' && N[2] == 'd' && N[3] == '\\');

						// load surface if it has an alpha channel or has "implicit lighting" flag
						BT.dwWidth				= BT.THM.width;
						BT.dwHeight				= BT.THM.height;
						BT.bHasAlpha			= BT.THM.HasAlphaChannel();
						if (!bLOD) 
						{
							if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
							{
								Logger.clMsg		("- loading: %s",N);
								u32			w=0, h=0;
								BT.pSurface = Surface_Load(N,w,h); 
								//R_ASSERT2	(BT.pSurface,"Can't load surface");
								if (!BT.pSurface)
								{
									Logger.clMsg("can't find tga texture: %s", N);
									is_tga_missing = true;
									continue;
								}
								if ((w != BT.dwWidth) || (h != BT.dwHeight))
									Msg		("! THM doesn't correspond to the texture: %dx%d -> %dx%d", BT.dwWidth, BT.dwHeight, w, h);
								BT.Vflip	();
							} else {
								// Free surface memory
							}
						}
					}
					// save all the stuff we've created
					g_textures.push_back	(BT);
				}
				R_ASSERT2(!is_thm_missing, "Some of required thm's are missing. Please, see log for details.");
				R_ASSERT2(!(is_tga_missing || is_thm_deprecated), "Some of required tga are missing/deprecated. Please, see log for details.");
			}
		}
	}
	
	// Load lights
	{
		xr_strconcat(N,name,"build.prj");
		IReader& fs = *FS.r_open(N);

		// Lights (Static)
		{
			IReader* F = fs.open_chunk(EB_Light_static);
			b_light_static	temp;
			u32 cnt		= F->length()/sizeof(temp);
			for				(u32 i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->r		(&temp,sizeof(temp));
				Flight&		L = temp.data;
				if (_abs(L.range) > 10000.f) {
					Msg		("! BAD light range : %f",L.range);
					L.range	= L.range > 0.f ? 10000.f : -10000.f;
				}

				// type
				RL.type	= (L.type == D3DLIGHT_DIRECTIONAL) ? LT_DIRECT : LT_POINT;

				// generic properties
				RL.position.set				(L.position);
				RL.direction.normalize_safe	(L.direction);
				RL.range				=	L.range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L.attenuation0;
				RL.attenuation1			=	L.attenuation1;
				RL.attenuation2			=	L.attenuation2;

				RL.amount				=	L.diffuse.magnitude_rgb	();
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);

				// place into layer
				if (!temp.controller_ID)	g_lights.push_back		(RL);
			}
			FS.r_close(F);
			fs.close();
		}
	}
	
	// Load initial map from the Level Editor
	{
		string_path			file_name;
		xr_strconcat(file_name,name,"build.aimap");
		IReader				*F = FS.r_open(file_name);
		R_ASSERT2			(F, file_name);

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_VERSION));
		R_ASSERT			(F->r_u16() == E_AIMAP_VERSION);

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_BOX));
		F->r				(&LevelBB,sizeof(LevelBB));

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_PARAMS));
		F->r				(&g_params,sizeof(g_params));

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_NODES));
		u32					N = F->r_u32();
		R_ASSERT2			(N < MAX_AI_NODES - 1,"Too many nodes!");
		g_nodes.resize		(N);

		hdrNODES			H;
		H.version			= XRAI_CURRENT_VERSION;
		H.count				= N+1;
		H.size				= g_params.fPatchSize;
		H.size_y			= 1.f;
		H.aabb				= LevelBB;
		
		typedef unsigned char NodeLink[3];
		for (u32 i=0; i<N; i++) {
			NodeLink			id;
			u16 				pl;
			SNodePositionOld 	_np;
			NodePosition 		np;
			
			for (int j=0; j<4; ++j) {
				F->r			(&id,3);
				g_nodes[i].n[j]	= (*LPDWORD(&id)) & 0x00ffffff;
			}

			pl				= F->r_u16();
			pvDecompress	(g_nodes[i].Plane.n,pl);
			F->r			(&_np,sizeof(_np));
			CNodePositionConverter(_np,H,np);
			g_nodes[i].Pos	= vertex_position(np,LevelBB,g_params);

			g_nodes[i].Plane.build(g_nodes[i].Pos,g_nodes[i].Plane.n);
		}

		F->close			();

		if (!strstr(Core.Params,"-keep_temp_files"))
			DeleteFile		(file_name);
	}
}
