#pragma once

class SimulatorStates;

#include "vkStateCache.h"
#include "vkSamplerStateCache.h"
#include "../xrRender/Shader.h"

class vkState
{
	//	Public interface
public:
	vkState();	//	These have to be private bu new/xr_delete don't support this
	~vkState();

	static vkState* Create(SimulatorStates& state_code, const SPass& pass);

	//	DX9 unified interface
	HRESULT Apply();
	void	Release();

	//	DX10 specific
	void	UpdateStencilRef(UINT Ref) { m_uiStencilRef = Ref; }
	void	UpdateAlphaRef(UINT Ref) { m_uiAlphaRef = Ref; }

	// VK specific
	void	GetDesc(D3D_RASTERIZER_DESC* pDesc);
	void	GetDesc(D3D_DEPTH_STENCIL_DESC* pDesc);
	void	GetDesc(D3D_BLEND_DESC* pDesc);

	//	User restricted interface
private:
	typedef	vkSamplerStateCache::HArray tSamplerHArray;

private:
	static void InitSamplers(tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex);

private:
	//	All states are supposed to live along all application lifetime
	vkStateCache				m_StateCache;

	tSamplerHArray				m_VSSamplers;
	tSamplerHArray				m_PSSamplers;
	tSamplerHArray				m_GSSamplers;
#ifdef USE_DX11
	tSamplerHArray				m_CSSamplers;
	tSamplerHArray				m_HSSamplers;
	tSamplerHArray				m_DSSamplers;
#endif

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

	D3D_RASTERIZER_DESC			m_RDesc;
	D3D_DEPTH_STENCIL_DESC		m_DSDesc;
	D3D_BLEND_DESC				m_BDesc;

	VkPipelineShaderStageCreateInfo m_Stages[6];
	VkPipelineVertexInputStateCreateInfo m_Input;

	//	Private data
private:
};
