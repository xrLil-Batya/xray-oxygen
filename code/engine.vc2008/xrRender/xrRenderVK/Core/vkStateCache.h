#pragma once
#include "../xrRender/Utils/dxHashHelper.h"
#include "vkStateCacheUtils.h"

class vkStateCache
{
public:
	typedef	struct {
		D3D_RASTERIZER_DESC		m_RDesc;
		D3D_DEPTH_STENCIL_DESC	m_DSDesc;
		D3D_BLEND_DESC			m_BDesc;
	} StateDecs;

	//	Public interface
public:
	vkStateCache();
	~vkStateCache();

	void			Init(const VkGraphicsPipelineCreateInfo& info);
	void			ClearStateArray();

	VkPipeline	GetState(SimulatorStates& state_code);
	VkPipeline	GetState(StateDecs& desc);
	//	Can be called on device destruction only!
	//	dx10State holds weak links on manager's states and 
	//	won't understand that state was destroyed
	//void	FlushStates();
	//	Private functionality

	//	Private declarations
private:
	struct StateRecord
	{
		u32						m_crc;
		StateDecs				m_desc;
		VkPipeline				m_pState;
	};

private:
	void			CreateState(StateDecs desc, VkPipeline* pState);
	VkPipeline		FindState(const StateDecs& desc, u32 StateCRC);

	//	Private data
private:
	//	This must be cleared on device destroy
	xr_vector<StateRecord>			m_StateArray;
	VkPipeline						m_Pipeline;
	VkGraphicsPipelineCreateInfo	m_PipelineInfo;
};

using vkStateUtils::operator==;

VkPipeline vkStateCache::GetState(SimulatorStates& state_code)
{
	StateDecs desc;
	vkStateUtils::ResetDescription(desc.m_RDesc);
	vkStateUtils::ResetDescription(desc.m_DSDesc);
	vkStateUtils::ResetDescription(desc.m_BDesc);
	state_code.UpdateDesc(desc.m_RDesc);
	state_code.UpdateDesc(desc.m_DSDesc);
	state_code.UpdateDesc(desc.m_BDesc);

	return GetState(desc);
}

VkPipeline vkStateCache::GetState(StateDecs& desc)
{
	VkPipeline	pResult;

	vkStateUtils::ValidateState(desc.m_RDesc);
	vkStateUtils::ValidateState(desc.m_DSDesc);
	vkStateUtils::ValidateState(desc.m_BDesc);

	dxHashHelper hash;
	vkStateUtils::GetHash(hash, desc.m_RDesc);
	vkStateUtils::GetHash(hash, desc.m_DSDesc);
	vkStateUtils::GetHash(hash, desc.m_BDesc);
	u32 crc = hash.GetHash();

	pResult = FindState(desc, crc);

	if (!pResult)
	{
		StateRecord rec;
		rec.m_crc = crc;
		CreateState(desc, &rec.m_pState);
		pResult = rec.m_pState;
		m_StateArray.push_back(rec);
	}

	return pResult;
}

VkPipeline vkStateCache::FindState(const StateDecs& desc, u32 StateCRC)
{
	u32 res = 0xffffffff;
	for (u32 i = 0; i < m_StateArray.size(); ++i)
	{
		if (m_StateArray[i].m_crc == StateCRC)
		{
			StateDecs	descCandidate = m_StateArray[i].m_desc;
			//if ( !memcmp(&descCandidate, &desc, sizeof(desc)) )
			if (descCandidate.m_RDesc == desc.m_RDesc &&
				descCandidate.m_DSDesc == desc.m_DSDesc &&
				descCandidate.m_BDesc == desc.m_BDesc)
				//break;
			//	TEST
			{
				res = i;
				break;
			}
			//else
			//{
			//	VERIFY(0);
			//}
		}
	}

	if (res != 0xffffffff)
		return m_StateArray[res].m_pState;
	else
		return NULL;
	/*
	if (i!=m_StateArray.size())
		return m_StateArray[i].m_pState;
	else
		return NULL;
		*/
}
