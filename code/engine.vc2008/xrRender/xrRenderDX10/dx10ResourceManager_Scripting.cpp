#include "stdafx.h"
#pragma hdrstop

#include	"../../xrEngine/Render.h"
#include	"../xrRender/ResourceManager.h"
#include	"../xrRender/tss.h"
#include	"../xrRender/blenders/blender.h"
#include	"../xrRender/blenders/blender_recorder.h"
//	adopt_compiler don't have = operator And it can't have = operator
#include	<luabind\luabind.hpp>
#include	"luabind/return_reference_to_policy.hpp"
#include	"../xrRender/dxRenderDeviceRender.h"
#include "../../xrScripts/lua_traceback.hpp"

using namespace luabind;

class	adopt_dx10options
{
public:
	bool	_dx10_msaa_alphatest_atoc() { return(RImplementation.o.dx10_msaa_alphatest == CRender::MSAA_ATEST_DX10_0_ATOC); }
};

// wrapper
class	adopt_dx10sampler
{
	CBlender_Compile*		m_pC;
	u32						m_SI;	//	Sampler index
public:
	adopt_dx10sampler	(CBlender_Compile*	C, u32 SamplerIndex)	: m_pC(C), m_SI(SamplerIndex)	{ if (u32(-1)==m_SI) m_pC=0;}
	adopt_dx10sampler	(const adopt_dx10sampler&	_C)				: m_pC(_C.m_pC), m_SI(_C.m_SI)	{ if (u32(-1)==m_SI) m_pC=0;}
};
#pragma warning( push )
#pragma warning( disable : 4512)
// wrapper																																					
class	adopt_compiler																																		
{
	CBlender_Compile*		C;
	bool					&m_bFirstPass;

	void					TryEndPass()												{	if (!m_bFirstPass) C->r_End(); m_bFirstPass = false;}
public:
	adopt_compiler			(CBlender_Compile*	_C, bool& bFirstPass)	: C(_C), m_bFirstPass(bFirstPass)		{ m_bFirstPass = true;}
	adopt_compiler			(const adopt_compiler&	_C)	: C(_C.C), m_bFirstPass(_C.m_bFirstPass){ }

	adopt_compiler&			_options		(int	P,		bool	S)				{	C->SetParams		(P,S);					return	*this;		}
	adopt_compiler&			_o_emissive		(bool	E)								{	C->SH->flags.bEmissive=E;					return	*this;		}
	adopt_compiler&			_o_distort		(bool	E)								{	C->SH->flags.bDistort=E;					return	*this;		}
	adopt_compiler&			_o_wmark		(bool	E)								{	C->SH->flags.bWmark=E;						return	*this;		}
	adopt_compiler&			_pass			(LPCSTR	vs,		LPCSTR ps)				{	TryEndPass();	C->r_Pass(vs,ps,true);		return	*this;		}
	adopt_compiler&			_passgs			(LPCSTR	vs,		LPCSTR	gs,		LPCSTR ps){	TryEndPass();	C->r_Pass(vs,gs,ps,true);	return	*this;		}
	adopt_compiler&			_passcs(LPCSTR	vs, LPCSTR	cs) { TryEndPass();	C->r_Pass(vs, cs, true);	return	*this; }
	adopt_compiler&			_fog			(bool	_fog)							{	C->PassSET_LightFog	(FALSE,_fog);			return	*this;		}
	adopt_compiler&			_ZB				(bool	_test,	bool _write)			{	C->PassSET_ZB		(_test,_write);			return	*this;		}
	adopt_compiler&			_blend			(bool	_blend, u32 abSRC, u32 abDST)	{	C->PassSET_ablend_mode(_blend,abSRC,abDST);	return 	*this;		}
	adopt_compiler&			_aref			(bool	_aref,  u32 aref)				{	C->PassSET_ablend_aref(_aref,aref);			return 	*this;		}
	adopt_compiler&			_dx10texture	(LPCSTR _resname, LPCSTR _texname)		{	C->r_dx10Texture(_resname, _texname);		return	*this;		}
	adopt_dx10sampler		_dx10sampler	(LPCSTR _name)							{	u32 s = C->r_dx10Sampler(_name);			return	adopt_dx10sampler(C,s);	}

	//	DX10 specific
	adopt_compiler&			_dx10color_write_enable (bool cR, bool cG, bool cB, bool cA)		{	C->r_ColorWriteEnable(cR, cG, cB, cA);		return	*this;		}
	adopt_compiler&			_dx10Stencil	(bool Enable, u32 Func, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail) {C->r_Stencil(Enable, Func, Mask, WriteMask, Fail, Pass, ZFail);		return	*this;		}
	adopt_compiler&			_dx10StencilRef	(u32 Ref) {C->r_StencilRef(Ref);		return	*this;		}
	adopt_compiler&			_dx10ATOC		(bool Enable)							{	C->RS.SetRS( XRDX10RS_ALPHATOCOVERAGE, Enable);	return *this;	}
	adopt_compiler&			_dx10ZFunc		(u32 Func)								{	C->RS.SetRS	( D3DRS_ZFUNC, Func);			return	*this;		}
	adopt_dx10options		_dx10Options	()										{	return adopt_dx10options();										};
};
#pragma warning( pop )

class	adopt_blend
{
public:
};

class	adopt_cmp_func
{
public:
};

class	adopt_stencil_op
{
public:
};


void LuaLog(LPCSTR caMessage)
{
	Log(caMessage);
}

// export
#include "../../xrScripts/VMLua.h"
//MatthewKush to all: I can do EVERYTHING from LUA from now on if you add the following options:
//alpha-blending, _passCS, ability to create my own sampler's and sampler states
void	CResourceManager::LS_Load()
{
    luaVM = xr_new<CVMLua>();
	function(luaVM->LSVM(), "log",	LuaLog);
	module(luaVM->LSVM())
	[
		class_<adopt_dx10options>("_dx10options")
		.def("dx10_msaa_alphatest_atoc",		&adopt_dx10options::_dx10_msaa_alphatest_atoc),

		class_<adopt_dx10sampler>("_dx10sampler"),

		class_<adopt_compiler>("_compiler")
			.def(								constructor<const adopt_compiler&>())
			.def("begin",						&adopt_compiler::_pass			,return_reference_to<1>())
			.def("begin",						&adopt_compiler::_passgs		,return_reference_to<1>())
		    .def("cs_begin", &adopt_compiler::_passcs, return_reference_to<1>())
			.def("sorting",						&adopt_compiler::_options		,return_reference_to<1>())
			.def("emissive",					&adopt_compiler::_o_emissive	,return_reference_to<1>())
			.def("distort",						&adopt_compiler::_o_distort		,return_reference_to<1>())
			.def("wmark",						&adopt_compiler::_o_wmark		,return_reference_to<1>())
			.def("fog",							&adopt_compiler::_fog			,return_reference_to<1>())
			.def("zb",							&adopt_compiler::_ZB			,return_reference_to<1>())
			.def("blend",						&adopt_compiler::_blend			,return_reference_to<1>())
			.def("aref",						&adopt_compiler::_aref			,return_reference_to<1>())
			//	For compatibility only
			.def("dx10color_write_enable",		&adopt_compiler::_dx10color_write_enable,return_reference_to<1>())
			.def("color_write_enable",			&adopt_compiler::_dx10color_write_enable,return_reference_to<1>())
			.def("dx10texture",					&adopt_compiler::_dx10texture	,return_reference_to<1>())
			.def("dx10stencil",					&adopt_compiler::_dx10Stencil	,return_reference_to<1>())
			.def("dx10stencil_ref",				&adopt_compiler::_dx10StencilRef,return_reference_to<1>())
			.def("dx10atoc",					&adopt_compiler::_dx10ATOC		,return_reference_to<1>())
			.def("dx10zfunc",					&adopt_compiler::_dx10ZFunc		,return_reference_to<1>())			

			.def("dx10sampler",					&adopt_compiler::_dx10sampler		)	// returns sampler-object
			.def("dx10Options",					&adopt_compiler::_dx10Options		),	// returns options-object			


		class_<adopt_blend>("blend")
			.enum_("blend")
			[
				value("zero", int(D3D11_BLEND_ZERO)),
				value("one", int(D3D11_BLEND_ONE)),
				value("srccolor", int(D3DBLEND_SRCCOLOR)),
				value("invsrccolor", int(D3DBLEND_INVSRCCOLOR)),
				value("srcalpha", int(D3D11_BLEND_SRC_ALPHA)),
				value("invsrcalpha", int(D3D11_BLEND_INV_SRC_ALPHA)),
				value("destalpha", int(D3DBLEND_DESTALPHA)),
				value("invdestalpha", int(D3DBLEND_INVDESTALPHA)),
				value("destcolor", int(D3DBLEND_DESTCOLOR)),
				value("invdestcolor", int(D3DBLEND_INVDESTCOLOR)),
				value("srcalphasat", int(D3DBLEND_SRCALPHASAT))
			],

			class_<adopt_cmp_func>("cmp_func")
			.enum_("cmp_func")
			[
				value("never", int(D3D11_COMPARISON_NEVER)),
				value("less", int(D3D11_COMPARISON_LESS)),
				value("equal", int(D3D11_COMPARISON_EQUAL)),
				value("lessequal", int(D3D11_COMPARISON_LESS_EQUAL)),
				value("greater", int(D3D11_COMPARISON_GREATER)),
				value("notequal", int(D3D11_COMPARISON_NOT_EQUAL)),
				value("greaterequal", int(D3D11_COMPARISON_GREATER_EQUAL)),
				value("always", int(D3D11_COMPARISON_ALWAYS))
			],

			class_<adopt_stencil_op>("stencil_op")
			.enum_("stencil_op")
			[
				value("keep", int(D3D11_STENCIL_OP_KEEP)),
				value("zero", int(D3D11_STENCIL_OP_ZERO)),
				value("replace", int(D3D11_STENCIL_OP_REPLACE)),
				value("incrsat", int(D3D11_STENCIL_OP_INCR_SAT)),
				value("decrsat", int(D3D11_STENCIL_OP_DECR_SAT)),
				value("invert", int(D3D11_STENCIL_OP_INVERT)),
				value("incr", int(D3D11_STENCIL_OP_INCR)),
				value("decr", int(D3D11_STENCIL_OP_DECR))
			]
		];

	// load shaders
	xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", ::Render->getShaderPath(), FS_ListFiles | FS_RootOnly);
	VERIFY(folder);
	for (u32 it = 0; it < folder->size(); it++)
	{
		string_path namesp, fn;
		xr_strcpy(namesp, (*folder)[it]);

		if (!strext(namesp) || xr_strcmp(strext(namesp), ".lua"))
			continue;

		*strext(namesp) = 0;

		if (!namesp[0])
			xr_strcpy(namesp, "_G");

		xr_strconcat( fn, ::Render->getShaderPath(), (*folder)[it]);
		FS.update_path(fn, "$game_shaders$", fn);

		try
		{
			luaVM->LoadFileIntoNamespace(fn, namesp, true);
		}
		catch (...)
		{
			Log(lua_tostring(luaVM->LSVM(), -1));
		}
	}
	FS.file_list_close(folder);
}

void	CResourceManager::LS_Unload			()
{
	try
	{
		xr_delete(luaVM);
	}
	catch (...)
	{
		Msg("[ERROR] Error lua vm closed...");
	}
}

BOOL CResourceManager::_lua_HasShader(LPCSTR s_shader)
{
	string256 undercorated;
	for (int i = 0, l = xr_strlen(s_shader) + 1; i < l; ++i)
		undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];

	// Check default workflow functions
	bool bHasShader = (luaVM->IsObjectPresent(undercorated, "normal", LUA_TFUNCTION) || luaVM->IsObjectPresent(undercorated, "l_special", LUA_TFUNCTION));

	// If not found - try to find new ones
	if (!bHasShader)
	{
		for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
		{
			string16 buff;
			xr_sprintf(buff, sizeof(buff), "element_%d", i);
			if (luaVM->IsObjectPresent(undercorated, buff, LUA_TFUNCTION))
			{
				bHasShader = true;
				break;
			}
		}
	}
	return bHasShader;
}

Shader*	CResourceManager::_lua_Create(LPCSTR d_shader, LPCSTR s_textures)
{
	CBlender_Compile C;
	Shader S;

	// undecorate
	string256 undercorated;
	for (int i = 0, l = xr_strlen(d_shader) + 1; i < l; ++i)
		undercorated[i] = ('\\' == d_shader[i]) ? '_' : d_shader[i];

	LPCSTR s_shader = undercorated;

	// Access to template
	C.BT				= NULL;
	C.bEditor			= FALSE;
	C.bDetail			= FALSE;

	// Prepare
	_ParseList			(C.L_textures,	s_textures);
	C.detail_texture	= NULL;
	C.detail_scaler		= NULL;

	// Choose workflow here: old (using named stages) or new (explicitly declaring stage number)
	bool bUseNewWorkflow = false;

	for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
	{
		string16 buff;
		xr_sprintf(buff, sizeof(buff), "element_%d", i);
		if (luaVM->IsObjectPresent(s_shader, buff, LUA_TFUNCTION))
		{
			C.iElement	= i;
			C.bDetail	= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
			S.E[i]		= C._lua_Compile(s_shader, buff);

			bUseNewWorkflow = true;
		}
	}

	if (!bUseNewWorkflow)
	{
		// Compile element	(LOD0 - HQ)
		if (luaVM->IsObjectPresent(s_shader,"normal_hq", LUA_TFUNCTION))
		{
			// Analyze possibility to detail this shader
			C.iElement			= 0;
			C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);

			if (C.bDetail)		S.E[0]	= C._lua_Compile(s_shader,"normal_hq");
			else				S.E[0]	= C._lua_Compile(s_shader,"normal");
		}
		else if (luaVM->IsObjectPresent(s_shader, "normal", LUA_TFUNCTION))
		{
			C.iElement			= 0;
			C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
			S.E[0]				= C._lua_Compile(s_shader, "normal");
		}

		// Compile element	(LOD1)
		if (luaVM->IsObjectPresent(s_shader,"normal", LUA_TFUNCTION))
		{
			C.iElement			= 1;
			C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
			S.E[1]				= C._lua_Compile(s_shader,"normal");
		}

		// Compile element
		if (luaVM->IsObjectPresent(s_shader,"l_point", LUA_TFUNCTION))
		{
			C.iElement			= 2;
			C.bDetail			= FALSE;
			S.E[2]				= C._lua_Compile(s_shader,"l_point");;
		}

		// Compile element
		if (luaVM->IsObjectPresent(s_shader,"l_spot", LUA_TFUNCTION))
		{
			C.iElement			= 3;
			C.bDetail			= FALSE;
			S.E[3]				= C._lua_Compile(s_shader,"l_spot");;
		}

		// Compile element
		if (luaVM->IsObjectPresent(s_shader,"l_special", LUA_TFUNCTION))
		{
			C.iElement			= 4;
			C.bDetail			= FALSE;
			S.E[4]				= C._lua_Compile(s_shader,"l_special");
		}
	}

	// Search equal in shaders array
	for (Shader* pShader : v_shaders)
	{
		if (S.equal(pShader))
			return pShader;
	}

	// Create _new_ entry
	Shader* N = xr_new<Shader>(S);
	N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	v_shaders.push_back(N);
	return N;
}

ShaderElement*		CBlender_Compile::_lua_Compile	(LPCSTR namesp, LPCSTR name)
{
	ShaderElement		E;
	SH =				&E;
	RS.Invalidate		();

	// Compile
	LPCSTR				t_0		= *L_textures[0]			? *L_textures[0] : "null";
	LPCSTR				t_1		= (L_textures.size() > 1)	? *L_textures[1] : "null";
	LPCSTR				t_d		= detail_texture			? detail_texture : "null" ;

    CVMLua* LSVM = dxRenderDeviceRender::Instance().Resources->luaVM;
	object				shader	= get_globals(LSVM->LSVM())[namesp];
	functor<void>		element	= object_cast<functor<void> >(shader[name]);
	bool				bFirstPass = false;
	adopt_compiler		ac		= adopt_compiler(this, bFirstPass);
	element						(ac,t_0,t_1,t_d);
	r_End				();
	ShaderElement*	_r	= dxRenderDeviceRender::Instance().Resources->_CreateElement(E);
	return			_r;
}
