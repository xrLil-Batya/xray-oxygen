#include "stdafx.h"
#include "vkSamplerStateCache.h"
#include "vkStateCacheUtils.h"

using vkStateUtils::operator==;

vkSamplerStateCache	SSManager;

vkSamplerStateCache::vkSamplerStateCache() :
	m_uiMaxAnisotropy(1)
{
	static const int iMaxRSStates = 10;
	m_StateArray.reserve(iMaxRSStates);
	ResetDeviceState();
}

vkSamplerStateCache::~vkSamplerStateCache()
{
	ClearStateArray();
}

vkSamplerStateCache::SHandle vkSamplerStateCache::GetState(D3D_SAMPLER_DESC& desc)
{
	SHandle	hResult;

	//	MaxAnisitropy is reset by ValidateState if not aplicable
	//	to the filter mode used.
	desc.maxAnisotropy = m_uiMaxAnisotropy;

	vkStateUtils::ValidateState(desc);

	//u32 crc = vkStateUtils::GetHash(desc);

	//hResult = FindState(desc, crc);

	if (hResult == hInvalidHandle)
	{
		StateRecord rec;
		//rec.m_crc = crc;
		CreateState(desc, &rec.m_pState);
		hResult = m_StateArray.size();
		m_StateArray.push_back(rec);
	}

	return hResult;
}

void vkSamplerStateCache::CreateState(StateDecs desc, IDeviceState** ppIState)
{
	//HW.pDevice->CreateSamplerState(&desc, ppIState);
}

vkSamplerStateCache::SHandle vkSamplerStateCache::FindState(const StateDecs& desc, u32 StateCRC)
{
	u32 res = 0xffffffff;
	u32 i = 0;
	while (i < m_StateArray.size())
	{
		if (m_StateArray[i].m_crc == StateCRC)
		{
			StateDecs	descCandidate;
			//m_StateArray[i].m_pState->GetDesc(&descCandidate);
			if (descCandidate == desc)
			{
				res = i;
				break;
			}
		}
		i++;
	}

	return res != 0xffffffff ? i : (u32)hInvalidHandle;
}

void vkSamplerStateCache::ClearStateArray()
{
	for (u32 i = 0; i < m_StateArray.size(); ++i)
	{
		//_RELEASE(m_StateArray[i].m_pState);
	}

	m_StateArray.clear();
}

void vkSamplerStateCache::PrepareSamplerStates(
	HArray &samplers,
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT],
	SHandle				pCurrentState[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT],
	u32	&uiMin,
	u32	&uiMax
) const
{
	//	It seems that sizeof pSS is 4 wor win32!
	ZeroMemory(pSS, sizeof(pSS[0])*D3D_COMMONSHADER_SAMPLER_SLOT_COUNT);

	for (u32 i = 0; i < samplers.size(); ++i)
	{
		if (samplers[i] != hInvalidHandle)
		{
			VERIFY(samplers[i] < m_StateArray.size());
			pSS[i] = m_StateArray[samplers[i]].m_pState;
		}
	}

	uiMin = 0;
	uiMax = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
}

void vkSamplerStateCache::VSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aVSSamplers, uiMin, uiMax);
	//HW.pContext->VSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}

void vkSamplerStateCache::PSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aPSSamplers, uiMin, uiMax);
	//HW.pContext->PSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}

void vkSamplerStateCache::GSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aGSSamplers, uiMin, uiMax);
	//HW.pContext->GSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}

#ifdef USE_DX11
void vkSamplerStateCache::HSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aHSSamplers, uiMin, uiMax);
	HW.pContext->HSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}

void vkSamplerStateCache::DSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aDSSamplers, uiMin, uiMax);
	HW.pContext->DSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}

void vkSamplerStateCache::CSApplySamplers(HArray &samplers)
{
	ID3DSamplerState	*pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	u32 uiMin;
	u32 uiMax;
	PrepareSamplerStates(samplers, pSS, m_aCSSamplers, uiMin, uiMax);
	HW.pContext->CSSetSamplers(uiMin, uiMax - uiMin + 1, &pSS[uiMin]);
}
#endif


void vkSamplerStateCache::SetMaxAnisotropy(UINT uiMaxAniso)
{
	clamp(uiMaxAniso, (u32)1, (u32)16);

	if (m_uiMaxAnisotropy == uiMaxAniso)
		return;

	m_uiMaxAnisotropy = uiMaxAniso;

	for (u32 i = 0; i < m_StateArray.size(); ++i)
	{
		StateRecord	&rec = m_StateArray[i];
		StateDecs	desc;

		//rec.m_pState->GetDesc(&desc);

		//	MaxAnisitropy is reset by ValidateState if not aplicable
		//	to the filter mode used.
		//	Reason: all checks for aniso applicability are done
		//	in ValidateState.
		desc.maxAnisotropy = m_uiMaxAnisotropy;
		vkStateUtils::ValidateState(desc);

		//	This can cause fragmentation if called too often
		//rec.m_pState->Release();
		CreateState(desc, &rec.m_pState);
	}
}

void vkSamplerStateCache::ResetDeviceState()
{
	for (int i = 0; i < sizeof(m_aPSSamplers) / sizeof(m_aPSSamplers[0]); ++i)
	{
		m_aPSSamplers[i] = (SHandle)hInvalidHandle;
		m_aVSSamplers[i] = (SHandle)hInvalidHandle;
		m_aGSSamplers[i] = (SHandle)hInvalidHandle;
#ifdef USE_DX11
		m_aHSSamplers[i] = (SHandle)hInvalidHandle;
		m_aDSSamplers[i] = (SHandle)hInvalidHandle;
		m_aCSSamplers[i] = (SHandle)hInvalidHandle;
#endif
	}
}