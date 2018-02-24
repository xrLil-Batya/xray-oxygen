// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "shader.h"
#include "tss_def.h"
#include "TextureDescrManager.h"


#include <functional>

// refs
struct lua_State;

class dx10ConstantBuffer;

#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef xr_map< K, T, P > N;	typedef N::iterator I;
// defs
class XRRENDER_COMMONS_API CResourceManager
{
private:
	struct str_pred : public std::function<bool(char*, char*)>	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	struct texture_detail	{
		const char*			T;
		ConstantTableSetup*	cs;
	};
public:
	DEFINE_MAP_PRED(const char*,IBlender*,		map_Blender,	map_BlenderIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CTexture*,		map_Texture,	map_TextureIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CMatrix*,		map_Matrix,		map_MatrixIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CConstant*,		map_Constant,	map_ConstantIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CRT*,			map_RT,			map_RTIt,			str_pred);
	//	DX10 cut DEFINE_MAP_PRED(const char*,CRTC*,			map_RTC,		map_RTCIt,			str_pred);
	DEFINE_MAP_PRED(const char*,VertexShaderRes*,			map_VS,			map_VSIt,			str_pred);
#if defined(USE_DX10) || defined(USE_DX11)
	DEFINE_MAP_PRED(const char*,GeometryShaderRes*,			map_GS,			map_GSIt,			str_pred);
#endif	//	USE_DX10
#ifdef USE_DX11
	DEFINE_MAP_PRED(const char*, HullShaderRes*,			map_HS,			map_HSIt,			str_pred);
	DEFINE_MAP_PRED(const char*, DomainShaderRes*,			map_DS,			map_DSIt,			str_pred);
	DEFINE_MAP_PRED(const char*, ComputeShaderRes*,			map_CS,			map_CSIt,			str_pred);
#endif

	DEFINE_MAP_PRED(const char*,PixelShaderRes*,			map_PS,			map_PSIt,			str_pred);
	DEFINE_MAP_PRED(const char*,texture_detail,	map_TD,			map_TDIt,			str_pred);
private:
	// data
	map_Blender											m_blenders;
	map_Texture											m_textures;
	map_Matrix											m_matrices;
	map_Constant										m_constants;
	map_RT												m_rtargets;
	//	DX10 cut map_RTC												m_rtargets_c;
	map_VS												m_vs;
	map_PS												m_ps;
#if defined(USE_DX10) || defined(USE_DX11)
	map_GS												m_gs;
#endif	//	USE_DX10
	map_TD												m_td;

	xr_vector<ShaderStateRes*>									v_states;
	xr_vector<ShaderDeclarationRes*>							v_declarations;
	xr_vector<SGeometry*>								v_geoms;
	xr_vector<ConstantTable*>						v_constant_tables;

#if defined(USE_DX10) || defined(USE_DX11)
	xr_vector<dx10ConstantBuffer*>						v_constant_buffer;
	xr_vector<SInputSignature*>							v_input_signature;
#endif	//	USE_DX10

	// lists
	xr_vector<ShaderTextureList*>							lst_textures;
	xr_vector<ShaderMatrixList*>								lst_matrices;
	xr_vector<ShaderConstantList*>							lst_constants;

	// main shader-array
	xr_vector<ShaderPass*>									v_passes;
	xr_vector<ShaderElement*>							v_elements;
	xr_vector<Shader*>									v_shaders;
	
	xr_vector<ref_texture>								m_necessary;
	// misc
public:
	CTextureDescrMngr									m_textures_description;
//.	CInifile*											m_textures_description;
	xr_vector<std::pair<shared_str,ConstantTableSetup*> >	v_constant_setup;
	lua_State*											LSVM;
	BOOL												bDeferredLoad;
private:
	//old LUA
	void							LS_Load				();
	void							LS_Unload			();

	//new Spectre
	void							Spectre_Load		();
public:
	// Miscelaneous
	void							_ParseList			(xrStringVec& dest, LPCSTR names);
	IBlender*						_GetBlender			(LPCSTR Name);
	IBlender* 						_FindBlender		(LPCSTR Name);
	void							_GetMemoryUsage		(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
	void							_DumpMemoryUsage	();
//.	BOOL							_GetDetailTexture	(LPCSTR Name, LPCSTR& T, R_constant_setup* &M);

	map_Blender&					_GetBlenders		()		{	return m_blenders;	}

	// Debug
	void							DBG_VerifyGeoms		();
	void							DBG_VerifyTextures	();

	// Editor cooperation
	void							ED_UpdateBlender	(LPCSTR Name, IBlender*		data);
	void							ED_UpdateMatrix		(LPCSTR Name, CMatrix*		data);
	void							ED_UpdateConstant	(LPCSTR Name, CConstant*	data);
#ifdef _EDITOR
	void							ED_UpdateTextures	(AStringVec* names);
#endif

	// Low level resource creation
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(const CTexture* T);

	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(const CMatrix*  M);

	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(const CConstant* C);

	ConstantTable*				_CreateConstantTable(ConstantTable& C);
	void							_DeleteConstantTable(const ConstantTable* C);

#if defined(USE_DX10) || defined(USE_DX11)
	dx10ConstantBuffer*				_CreateConstantBuffer(ID3DShaderReflectionConstantBuffer* pTable);
	void							_DeleteConstantBuffer(const dx10ConstantBuffer* pBuffer);

	SInputSignature*				_CreateInputSignature(ID3DBlob* pBlob);
	void							_DeleteInputSignature(const SInputSignature* pSignature);
#endif	//	USE_DX10

#ifdef USE_DX11
	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f, u32 SampleCount = 1, bool useUAV=false );
#else
	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f, u32 SampleCount = 1 );
#endif
	void							_DeleteRT			(const CRT*	RT	);

	//	DX10 cut CRTC*							_CreateRTC			(LPCSTR Name, u32 size,	D3DFORMAT f);
	//	DX10 cut void							_DeleteRTC			(const CRTC*	RT	);
#if defined(USE_DX10) || defined(USE_DX11)
	GeometryShaderRes*							_CreateGS			(LPCSTR Name);
	void							_DeleteGS			(const GeometryShaderRes*	GS	);
#endif	//	USE_DX10

#ifdef USE_DX11
	HullShaderRes*							_CreateHS			(LPCSTR Name);
	void							_DeleteHS			(const HullShaderRes*	HS	);

	DomainShaderRes*							_CreateDS			(LPCSTR Name);
	void							_DeleteDS			(const DomainShaderRes*	DS	);

    ComputeShaderRes*							_CreateCS			(LPCSTR Name);
	void							_DeleteCS			(const ComputeShaderRes*	CS	);
#endif	//	USE_DX10

	PixelShaderRes*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(const PixelShaderRes*	PS	);

	VertexShaderRes*							_CreateVS			(LPCSTR Name);
	void							_DeleteVS			(const VertexShaderRes*	VS	);

	ShaderPass*							_CreatePass			(const ShaderPass& proto);
	void							_DeletePass			(const ShaderPass* P	);

	// Shader compiling / optimizing
	ShaderStateRes*							_CreateState		(SimulatorStates& Code);
	void							_DeleteState		(const ShaderStateRes* SB);

	ShaderDeclarationRes*					_CreateDecl			(D3DVERTEXELEMENT9* dcl);
	void							_DeleteDecl			(const ShaderDeclarationRes* dcl);

	ShaderTextureList*					_CreateTextureList	(ShaderTextureList& L);
	void							_DeleteTextureList	(const ShaderTextureList* L);

	ShaderMatrixList*					_CreateMatrixList	(ShaderMatrixList& L);
	void							_DeleteMatrixList	(const ShaderMatrixList* L);

	ShaderConstantList*					_CreateConstantList	(ShaderConstantList& L);
	void							_DeleteConstantList	(const ShaderConstantList* L);

	ShaderElement*					_CreateElement		(ShaderElement& L);
	void							_DeleteElement		(const ShaderElement* L);

	Shader*							_cpp_Create			(LPCSTR		s_shader,	LPCSTR s_textures=0,	LPCSTR s_constants=0,	LPCSTR s_matrices=0);
	Shader*							_cpp_Create			(IBlender*	B,			LPCSTR s_shader=0,		LPCSTR s_textures=0,	LPCSTR s_constants=0, LPCSTR s_matrices=0);
	Shader*							_lua_Create			(LPCSTR		s_shader,	LPCSTR s_textures);
	BOOL							_lua_HasShader		(LPCSTR		s_shader);

	CResourceManager						()	: bDeferredLoad(TRUE){	}
	~CResourceManager						()	;

	void			OnDeviceCreate			(IReader* F);
	void			OnDeviceCreate			(LPCSTR name);
	void			OnDeviceDestroy			(BOOL   bKeepTextures);

	void			reset_begin				();
	void			reset_end				();

	// Creation/Destroying
	Shader*			Create					(LPCSTR s_shader=0, LPCSTR s_textures=0,	LPCSTR s_constants=0,	LPCSTR s_matrices=0);
	Shader*			Create					(IBlender*	B,		LPCSTR s_shader=0,		LPCSTR s_textures=0,	LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			Delete					(const Shader*		S	);
	void			RegisterConstantSetup	(LPCSTR name,		ConstantTableSetup* s)	{	v_constant_setup.push_back(std::make_pair(shared_str(name),s));	}

	SGeometry*		CreateGeom				(D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
	SGeometry*		CreateGeom				(u32 FVF				, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
	void			DeleteGeom				(const SGeometry* VS		);
	void			DeferredLoad			(BOOL E)					{ bDeferredLoad=E;	}
	void			DeferredUpload			();
//.	void			DeferredUnload			();
	void			StoreNecessaryTextures	();
	void			DestroyNecessaryTextures();
	void			Dump					(bool bBrief);
	
private:
#ifdef USE_DX11
	map_DS	m_ds;
	map_HS	m_hs;
	map_CS	m_cs;

	template<typename T>
	T& GetShaderMap();

	template<typename T>
	T* CreateShader(const char* name);

	template<typename T>
	void DestroyShader(const T* sh);

#endif	//	USE_DX10
};

#undef DEFINE_MAP_PRED