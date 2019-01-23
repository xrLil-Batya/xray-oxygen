#pragma once

class vkSamplerStateCache
{
public:
	enum
	{
		hInvalidHandle = 0xFFFFFFFF
	};

	//	State handle
	typedef	u32	SHandle;
	typedef	xr_vector<SHandle>	HArray;
public:
	vkSamplerStateCache();
	~vkSamplerStateCache();

	void	ClearStateArray();

	SHandle	GetState(D3D_SAMPLER_DESC& desc);

	void	VSApplySamplers(HArray &samplers);
	void	PSApplySamplers(HArray &samplers);
	void	GSApplySamplers(HArray &samplers);

	void	SetMaxAnisotropy(UINT uiMaxAniso);

	//	Marks all device sample as unused
	void	ResetDeviceState();

	//	Private declarations
private:
	typedef	ID3DSamplerState	IDeviceState;
	typedef	D3D_SAMPLER_DESC	StateDecs;

	struct StateRecord
	{
		u32					m_crc;
		IDeviceState*		m_pState;
	};

private:
	void	CreateState(StateDecs desc, IDeviceState** ppIState);
	SHandle	FindState(const StateDecs& desc, u32 StateCRC);

	void	PrepareSamplerStates(
		HArray &samplers,
		ID3DSamplerState	*pSS[16],
		SHandle pCurrentState[16],
		u32	&uiMin,
		u32	&uiMax
	) const;

	//	Private data
private:
	//	This must be cleared on device destroy
	xr_vector<StateRecord>	m_StateArray;

	SHandle					m_aPSSamplers[16];
	SHandle					m_aVSSamplers[16];
	SHandle					m_aGSSamplers[16];
#ifdef USE_DX11
	SHandle					m_aHSSamplers[16];
	SHandle					m_aDSSamplers[16];
	SHandle					m_aCSSamplers[16];
#endif

	u32						m_uiMaxAnisotropy;
};

extern	vkSamplerStateCache	SSManager;