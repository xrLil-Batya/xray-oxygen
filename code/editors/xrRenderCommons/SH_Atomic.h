#ifndef sh_atomicH
#define sh_atomicH
#pragma once
#include "xrCore/xr_resource.h"
#include "tss_def.h"

#if defined(USE_DX10) || defined(USE_DX11)
#include "../xrRenderDX10/StateManager/DirectXState.h"
#endif	//	USE_DX10

//#pragma pack(push,8)


//////////////////////////////////////////////////////////////////////////
// Atomic resources
//////////////////////////////////////////////////////////////////////////
#if defined(USE_DX10) || defined(USE_DX11)
struct XRRENDER_COMMONS_API SInputSignature : public xr_resource_flagged
{
	ID3DBlob*							signature;
	SInputSignature(ID3DBlob* pBlob);
	~SInputSignature();
};
typedef	resptr_core<SInputSignature,resptr_base<SInputSignature> >	ref_input_sign;
#endif	//	USE_DX10
//////////////////////////////////////////////////////////////////////////
struct XRRENDER_COMMONS_API VertexShaderRes : public xr_resource_named
{
	ID3DVertexShader*					vs;
	ConstantTable					constants;
#if defined(USE_DX10) || defined(USE_DX11)
	ref_input_sign						signature;
#endif	//	USE_DX10
	VertexShaderRes				();
	~VertexShaderRes			();
};
typedef	resptr_core<VertexShaderRes,resptr_base<VertexShaderRes> >	ref_vs;

//////////////////////////////////////////////////////////////////////////
struct XRRENDER_COMMONS_API PixelShaderRes : public xr_resource_named
{
	ID3DPixelShader*					ps;
	ConstantTable					constants;
	~PixelShaderRes			();
};
typedef	resptr_core<PixelShaderRes,resptr_base<PixelShaderRes> > ref_ps;

#if defined(USE_DX10) || defined(USE_DX11)
//////////////////////////////////////////////////////////////////////////
struct XRRENDER_COMMONS_API GeometryShaderRes : public xr_resource_named
{
	ID3DGeometryShader*					gs;
	ConstantTable					constants;
	~GeometryShaderRes			();
};
typedef	resptr_core<GeometryShaderRes,resptr_base<GeometryShaderRes> > ref_gs;
#endif	//	USE_DX10

#ifdef USE_DX11

struct XRRENDER_COMMONS_API HullShaderRes : public xr_resource_named
{
	ID3D11HullShader*					sh;
	ConstantTable					constants;
	~HullShaderRes			();
};
typedef	resptr_core< HullShaderRes, resptr_base<HullShaderRes> >	ref_hs;

struct XRRENDER_COMMONS_API DomainShaderRes : public xr_resource_named
{
	ID3D11DomainShader*					sh;
	ConstantTable					constants;
	~DomainShaderRes			();
};
typedef	resptr_core< DomainShaderRes, resptr_base<DomainShaderRes> >	ref_ds;

struct XRRENDER_COMMONS_API ComputeShaderRes : public xr_resource_named
{
	ID3D11ComputeShader*					sh;
	ConstantTable					constants;
	~ComputeShaderRes			();
};
typedef	resptr_core< ComputeShaderRes, resptr_base<ComputeShaderRes> >	ref_cs;

#endif

//////////////////////////////////////////////////////////////////////////
struct XRRENDER_COMMONS_API ShaderStateRes : public xr_resource_flagged
{
	ID3DState*							state;
	SimulatorStates						state_code;
	~ShaderStateRes			();
};
typedef	resptr_core<ShaderStateRes,resptr_base<ShaderStateRes> >	ref_state;

//////////////////////////////////////////////////////////////////////////
struct XRRENDER_COMMONS_API ShaderDeclarationRes : public xr_resource_flagged
{
#if defined(USE_DX10) || defined(USE_DX11)
	//	Maps input signature to input layout
	xr_map<ID3DBlob*, ID3DInputLayout*>		vs_to_layout;
	xr_vector<D3D_INPUT_ELEMENT_DESC>		dx10_dcl_code;
#else	//	USE_DX10	//	Don't need it: use ID3DInputLayout instead
					//	which is per ( declaration, VS input layout) pair
	IDirect3DVertexDeclaration9*		dcl;
#endif	//	USE_DX10

	//	Use this for DirectX10 to cache DX9 declaration for comparison purpose only
	xr_vector<D3DVERTEXELEMENT9>		dcl_code;
	~ShaderDeclarationRes	();
};
typedef	resptr_core<ShaderDeclarationRes,resptr_base<ShaderDeclarationRes> >	ref_declaration;

//#pragma pack(pop)
#endif //sh_atomicH
