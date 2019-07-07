// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class RENDER_API CHW : public pureAppActivate, public pureAppDeactivate
{
//	Functions section
public:
	CHW();
	~CHW();

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw, bool move_window);

	void					DestroyDevice			();

	void					Reset					(HWND hw);

	void					SelectResolution		(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed);
#ifdef USE_VK
	VkFormat				SelectFmtTarget();
	VkFormat				SelectFmtDepthStencil(VkFormat);
	u32						SelectRefresh(u32 dwWidth, u32 dwHeight, VkFormat fmt);
#endif
	void					ResizeWindowProc		(WORD h, WORD w);
	u32						SelectPresentInterval	();
	u32						SelectGPU				();

	void					FillVidModeList			();
	void					FreeVidModeList			();

//	Variables section
public:
#ifdef USE_DX11
	IDXGIAdapter1*			m_pAdapter;	//	pD3D equivalent
	ID3D11Device*			pDevice;	//	combine with DX9 pDevice via typedef
	ID3D11DeviceContext*    pContext;	//	combine with DX9 pDevice via typedef
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView*	pBaseRT;	//	combine with DX9 pBaseRT via typedef
	ID3D11DepthStencilView*	pBaseZB;
	ID3DUserDefinedAnnotation* pAnnotation;

	CHWCaps					Caps;

	D3D_DRIVER_TYPE			m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC	m_ChainDesc;	//	DevPP equivalent
	bool					m_bUsePerfhud;
	bool					m_bDX11_1;

	D3D_FEATURE_LEVEL		FeatureLevel;
	bool					IsFormatSupported		(DXGI_FORMAT fmt);
#elif defined(USE_VK)
	VkPhysicalDevice*		m_pAdapter;	//	pD3D equivalent
	VkDevice*				pDevice;	//	combine with DX9 pDevice via typedef
	VkInstance*				pContext;	//	combine with DX9 pDevice via typedef
	VkSwapchainKHR*			m_pSwapChain;
	VkImageView*			pBaseRT;	//	combine with DX9 pBaseRT via typedef
	VkImageView*			pBaseZB;
	bool					m_bUsePerfhud;

	CHWCaps					Caps;
	VkSwapchainKHR			m_ChainDesc;
#endif

	void	Validate() {};

	DXGI_RATIONAL	SelectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

	void			UpdateViews();
	virtual	void	OnAppActivate();
	virtual void	OnAppDeactivate();

	stats_manager			stats_manager;

private:
	bool					m_move_window;
};

extern RENDER_API CHW HW;
