#pragma once

#include "../shader_xrlc.h"
#include "../common/le_engine_communicate.hpp"
#include "../../xrcore/xrPool.h"

#include "xrfacedefs.h"
#include "xrdeflectordefs.h"
#include "b_build_texture.h"
#include "base_lighting.h"

namespace CDB 
{
	class MODEL;
	class CollectorPacked;
};
class CLightmap;
class xrMU_Model;
class xrMU_Reference;
class base_Vertex;
class base_Face;

struct	compilers_global_data
{
		xr_vector<b_BuildTexture>		_textures;
		xr_vector<b_material>			_materials;
		Shader_xrLC_LIB					_shaders;
		b_params						_g_params;
		base_lighting					_L_static;
		CDB::MODEL*						_RCAST_Model;
};


class	XRLC_LIGHT_API xrLC_GlobalData
{
	
		compilers_global_data			_cl_globs;

		CMemoryWriter					_err_invalid;
		CMemoryWriter					_err_multiedge;
		CMemoryWriter					_err_tjunction;
		xr_vector<CLightmap*>			_g_lightmaps;
		xr_vector<xrMU_Model*>			_mu_models;
		xr_vector<xrMU_Reference*>		_mu_refs;
		vecVertex						_g_vertices;
		vecFace							_g_faces;
		vecDefl							_g_deflectors;


		bool							_b_nosun;
		bool							_b_slmap;
		bool							_gl_linear;
private:
		bool							b_vert_not_register;
public:

public:
									xrLC_GlobalData	();
									~xrLC_GlobalData();
		IC xr_vector<b_BuildTexture>& textures		()		{	return _cl_globs._textures; }
		IC xr_vector<CLightmap*>	& lightmaps		()		{	return _g_lightmaps; }
		IC xr_vector<b_material>	& materials		()		{	return _cl_globs._materials; }
		IC Shader_xrLC_LIB			& shaders		()		{	return _cl_globs._shaders; }
		IC CMemoryWriter			&err_invalid	()		{	return _err_invalid; }
		IC CMemoryWriter			&err_multiedge	()		{	return _err_multiedge;  };
		IC CMemoryWriter			&err_tjunction	()		{	return _err_tjunction;  };
		IC b_params					&g_params		()		{	return _cl_globs._g_params; }
			
		Face						*create_face	()		;
		void						destroy_face	(Face* &f );

		Vertex						*create_vertex	()		;
		void						destroy_vertex	(Vertex* &f );

		void						vertices_isolate_and_pool_reload();

		vecVertex					&g_vertices		()		{	return	_g_vertices; }
		vecFace						&g_faces		()		{	return	_g_faces; }
		vecDefl						&g_deflectors	()		{	return	_g_deflectors; }
		bool						b_r_vertices	()		;
		bool						vert_construct_register(){	return !b_r_vertices() && !b_vert_not_register; }
		base_lighting				&L_static		()		{	return _cl_globs._L_static; }
		CDB::MODEL*					RCAST_Model		()		{	return _cl_globs._RCAST_Model; }
		xr_vector<xrMU_Model*>		&mu_models		()		{	return _mu_models; }
		xr_vector<xrMU_Reference*>	&mu_refs		()		{	return _mu_refs; }

		void						read_mu_models			( IReader &r );
		void						write_mu_models			( IWriter	&w ) const ;

		void						read_mu_model_refs		( IReader &r );
		void						write_mu_model_refs		( IWriter	&w ) const ;
		void						close_models_read		();
		void						close_models_write		()const ;


		bool						b_skiplmap		()		{	return _b_slmap; }
		bool						b_nosun			()		{	return _b_nosun; }
		bool						gl_linear		()		{	return _gl_linear; }
		void						b_nosun_set		(bool v){	_b_nosun = v; }
		void						b_skiplmap_set	(bool v){	_b_slmap = v; }
		void						initialize		()		;
		void						destroy_rcmodel	()		;

		void						create_rcmodel	(CDB::CollectorPacked& CL);

		void						clear_build_textures_surface();
		
		void						clear_build_textures_surface( const xr_vector<u32> &exept );

		void						set_faces_indexses		();
		void						set_vertices_indexses	();

		void						gl_mesh_clear			();
public:
		void						read_base				( IReader &r );
		void						write_base				( IWriter	&w ) const ;
		void						read					( IReader &r );
		void						write					( IWriter	&w ) const ;
		void						read_vertices			( IReader	&r );
		void						write_vertices			( IWriter	&w )const;
		void						read_lm_data			( IReader	&r );
		void						write_lm_data			( IWriter	&w )const;

		void						read_modes_color		( IReader	&r );
		void						write_modes_color		( IWriter	&w )const;

		void						read					( IReader &r, base_Face* &f);
		void						write					( IWriter &r, const base_Face *f ) const ;
		void						clear					();
		void						clear_mesh				();
		void						clear_mu_models			();	
		void						mu_models_calc_materials();
};													

extern "C" XRLC_LIGHT_API xrLC_GlobalData*	lc_global_data();
extern "C" XRLC_LIGHT_API void				create_global_data();
extern "C" XRLC_LIGHT_API void				destroy_global_data();
extern "C" XRLC_LIGHT_API u32				InvalideFaces();
		   XRLC_LIGHT_API void				ImplicitLighting(u32 thCount);

extern xrLC_GlobalData* data;
IC xrLC_GlobalData* inlc_global_data() { return data; }
static LPCSTR gl_data_net_file_name = "tmp_global_data";