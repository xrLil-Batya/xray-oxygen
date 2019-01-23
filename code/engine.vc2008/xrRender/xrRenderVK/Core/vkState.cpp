#include "stdafx.h"
#include "vkState.h"
#include "vkStateCache.h"
#include "../xrRender/Shader.h"

vkState::vkState() :
	m_uiStencilRef(UINT(-1)),
	m_uiAlphaRef(0)
{
	memset(&m_Stages, 0, sizeof(m_Stages));
	memset(&m_Input, 0, sizeof(m_Input));
}

vkState::~vkState()
{
	//	m_pRasterizerState is a weak link
	//	m_pDepthStencilState is a weak link
	//	m_pBlendState is a weak link
}

vkState* vkState::Create(SimulatorStates& state_code, const SPass& pass)
{
	vkState	*pState = new vkState();

	VkGraphicsPipelineCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	if (pass.vs)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pState->m_Stages[info.stageCount].module = pass.vs->vs;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}

	if (pass.ps)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pState->m_Stages[info.stageCount].module = pass.ps->ps;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}

	if (pass.gs)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		pState->m_Stages[info.stageCount].module = pass.gs->gs;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}

	if (pass.hs)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		pState->m_Stages[info.stageCount].module = pass.hs->sh;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}

	if (pass.ds)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		pState->m_Stages[info.stageCount].module = pass.ds->sh;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}

	if (pass.cs)
	{
		pState->m_Stages[info.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pState->m_Stages[info.stageCount].stage = VK_SHADER_STAGE_COMPUTE_BIT;
		pState->m_Stages[info.stageCount].module = pass.cs->sh;
		pState->m_Stages[info.stageCount].pName = "main";
		info.stageCount++;
	}
	info.pStages = pState->m_Stages;

	m_Input.pVertexAttributeDescriptions
		info.pVertexInputState = &m_Input;


	state_code.UpdateState(*pState);
	state_code.UpdateDesc(pState->m_RDesc);
	state_code.UpdateDesc(pState->m_DSDesc);
	state_code.UpdateDesc(pState->m_BDesc);

	//ID3DxxDevice::CreateSamplerState

	//	Create samplers here
	{
		InitSamplers(pState->m_VSSamplers, state_code, CTexture::rstVertex);
		InitSamplers(pState->m_PSSamplers, state_code, CTexture::rstPixel);
		InitSamplers(pState->m_GSSamplers, state_code, CTexture::rstGeometry);
#ifdef USE_DX11
		InitSamplers(pState->m_HSSamplers, state_code, CTexture::rstHull);
		InitSamplers(pState->m_DSSamplers, state_code, CTexture::rstDomain);
		InitSamplers(pState->m_CSSamplers, state_code, CTexture::rstCompute);
#endif
	}


	return pState;
}

HRESULT vkState::Apply()
{
	StateManager.SetState(this);
	if (m_uiStencilRef != -1)
		StateManager.SetStencilRef(m_uiStencilRef);
	StateManager.SetAlphaRef(m_uiAlphaRef);

	SSManager.GSApplySamplers(m_GSSamplers);
	SSManager.VSApplySamplers(m_VSSamplers);
	SSManager.PSApplySamplers(m_PSSamplers);
#ifdef USE_DX11
	SSManager.HSApplySamplers(m_HSSamplers);
	SSManager.DSApplySamplers(m_DSSamplers);
	SSManager.CSApplySamplers(m_CSSamplers);
#endif

	//	static const FLOAT BlendFactor[4] = {0.000f, 0.000f, 0.000f, 0.000f};
	//	static const UINT SampleMask = 0xffffffff;

	//	VERIFY(m_pRasterizerState);
	//	HW.pDevice->RSSetState(m_pRasterizerState);
	//	VERIFY(m_pDepthStencilState);
	//	HW.pDevice->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
	//	VERIFY(m_pBlendState);
	//	HW.pDevice->OMSetBlendState(m_pBlendState, BlendFactor, SampleMask);

	return S_OK;
}

void vkState::Release()
{
	vkState	*pState = this;
	xr_delete<vkState>(pState);
}

void vkState::InitSamplers(tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
	D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];

	for (int i = 0; i < D3D_COMMONSHADER_SAMPLER_SLOT_COUNT; ++i)
	{
		SamplerUsed[i] = false;
		vkStateUtils::ResetDescription(descArray[i]);
	}

	state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

	int iMaxSampler = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
	for (; iMaxSampler > -1; --iMaxSampler)
	{
		if (SamplerUsed[iMaxSampler])
			break;
	}

	if (iMaxSampler > -1)
	{
		SamplerArray.reserve(iMaxSampler + 1);
		for (int i = 0; i <= iMaxSampler; ++i)
		{
			if (SamplerUsed[i])
				SamplerArray.push_back(SSManager.GetState(descArray[i]));
			else
				SamplerArray.push_back(u32(vkSamplerStateCache::hInvalidHandle));
		}
	}
}

void vkState::GetDesc(D3D_RASTERIZER_DESC* pDesc)
{
	*pDesc = m_RDesc;
}

void vkState::GetDesc(D3D_DEPTH_STENCIL_DESC* pDesc)
{
	*pDesc = m_DSDesc;
}

void vkState::GetDesc(D3D_BLEND_DESC* pDesc)
{
	*pDesc = m_BDesc;
}