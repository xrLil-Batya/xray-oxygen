// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class  CHW
#if defined(USE_DX11) || defined(USE_VK)
	:	public pureAppActivate, 
		public pureAppDeactivate
#endif
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
#else
	D3DFORMAT				SelectFmtTarget();
	D3DFORMAT				SelectFmtDepthStencil(D3DFORMAT);
	u32						SelectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);
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
#else
	IDirect3D9* 			pD3D;		// D3D
	IDirect3DDevice9*		pDevice;	// render device

	IDirect3DSurface9*		pBaseRT;
	IDirect3DSurface9*		pBaseZB;
	CHWCaps					Caps;

	UINT					DevAdapter;
	D3DDEVTYPE				DevT;
	D3DPRESENT_PARAMETERS	DevPP;
	bool					IsFormatSupported		(D3DFORMAT fmt, DWORD type, DWORD usage);
#endif

#if defined(DEBUG) && (!defined(USE_DX11) || !defined(USE_VK))
	void	Validate() { VERIFY(pDevice); /*VERIFY(pD3D);*/ };
#else
	void	Validate() {};
#endif

#ifdef USE_DX11
	DXGI_RATIONAL	SelectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);
#endif

#if defined(USE_DX11) || defined(USE_VK)
	void			UpdateViews();
	virtual	void	OnAppActivate();
	virtual void	OnAppDeactivate();
#endif
#ifndef _MAYA_EXPORT
	stats_manager			stats_manager;
#endif
private:
	bool					m_move_window;
};

extern ECORE_API CHW		HW;
