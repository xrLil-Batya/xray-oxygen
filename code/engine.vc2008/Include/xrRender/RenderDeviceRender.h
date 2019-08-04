#pragma once

class IRenderDeviceRender
{
public:
	enum	DeviceState
	{
		dsOK = 0,
		dsLost,
		dsNeedReset,
		dsRemoved
	};

public:
	virtual ~IRenderDeviceRender() {;}
	virtual void	Copy(IRenderDeviceRender &_in) = 0;

	//	Gamma correction functions
	virtual float	GetGamma		() const		= 0;
	virtual float	GetBrightness	() const		= 0;
	virtual float	GetContrast		() const		= 0;
	virtual Fvector	GetBalance		() const		= 0;

	virtual void	SetGamma		(float fGamma)	= 0;
	virtual void	SetBrightness	(float fGamma)	= 0;
	virtual void	SetContrast		(float fGamma)	= 0;
	virtual void	SetBalance		(float r, float g, float b) = 0;
	virtual void	SetBalance		(Fvector &C)	= 0;
	virtual void	UpdateGamma		()				= 0;

	//	Destroy
	virtual void	OnDeviceDestroy( BOOL bKeepTextures) = 0;
	virtual void	ValidateHW() = 0;
	virtual void	DestroyHW() = 0;
	virtual void	Reset( HWND hWnd, u32 &dwWidth, u32 &dwHeight, float &fWidth_2, float &fHeight_2) = 0;
	//	Init
	virtual void	SetupStates() = 0;
	virtual void	OnDeviceCreate(LPCSTR shName) = 0;
	virtual void	Create( HWND hWnd, u32 &dwWidth, u32 &dwHeight, float &fWidth_2, float &fHeight_2, bool ) = 0;
	virtual void	SetupGPU( BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF) = 0;
	//	Overdraw
	virtual void	overdrawBegin() = 0;
	virtual void	overdrawEnd() = 0;

	//	Resources control
	virtual void	DeferredLoad(BOOL E) = 0;
	virtual void	ResourcesDeferredUpload() = 0;
	virtual void	ResourcesDestroyNecessaryTextures() = 0;
	virtual void	ResourcesStoreNecessaryTextures() = 0;

	//	HWSupport
	virtual bool	HWSupportsShaderYUV2RGB() = 0;

	//	Device state
	virtual DeviceState GetDeviceState() = 0;
	virtual BOOL	GetForceGPU_REF() = 0;
	virtual u32		GetCacheStatPolys() = 0;
	virtual void	Begin() = 0;
	virtual void	Clear() = 0;
	virtual void	End() = 0;
	virtual void	ClearTarget() = 0;
	virtual void	SetCacheXform(const Fmatrix &mView, const Fmatrix &mProject) = 0;
	virtual void	OnAssetsChanged() = 0;
};