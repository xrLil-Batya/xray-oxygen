// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class  CHW
#ifdef USE_DX11
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
	D3DFORMAT				SelectFmtTarget			();
	D3DFORMAT				SelectFmtDepthStencil	(D3DFORMAT);
	void					ResizeWindowProc		(WORD h, WORD w);
	u32						SelectPresentInterval	();
	u32						SelectGPU				();
	u32						SelectRefresh			(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);
	bool					IsFormatSupported		(D3DFORMAT fmt, DWORD type, DWORD usage);

	void					FillVidModeList			();
	void					FreeVidModeList			();

#ifdef DEBUG
#ifdef USE_DX11
	void	Validate(void)	{};
#else
	void	Validate(void)	{	VERIFY(pDevice); VERIFY(pD3D); };
#endif
#else
	void	Validate(void)	{};
#endif

//	Variables section
public:
#ifdef USE_DX11
	IDXGIAdapter1*			m_pAdapter;	//	pD3D equivalent
	ID3D11Device*			pDevice;	//	combine with DX9 pDevice via typedef
	ID3D11DeviceContext*    pContext;	//	combine with DX9 pDevice via typedef
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView*	pBaseRT;	//	combine with DX9 pBaseRT via typedef
	ID3D11DepthStencilView*	pBaseZB;

	CHWCaps					Caps;

	D3D_DRIVER_TYPE			m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC	m_ChainDesc;	//	DevPP equivalent
	bool					m_bUsePerfhud;
	D3D_FEATURE_LEVEL		FeatureLevel;
#else
	IDirect3D9* 			pD3D;		// D3D
	IDirect3DDevice9*		pDevice;	// render device

	IDirect3DSurface9*		pBaseRT;
	IDirect3DSurface9*		pBaseZB;

	CHWCaps					Caps;

	UINT					DevAdapter;
	D3DDEVTYPE				DevT;
	D3DPRESENT_PARAMETERS	DevPP;
#endif

#ifndef _MAYA_EXPORT
	stats_manager			stats_manager;
#endif
#ifdef USE_DX11
	void			UpdateViews();
	DXGI_RATIONAL	SelectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

	virtual	void	OnAppActivate();
	virtual void	OnAppDeactivate();
#endif

private:
	bool					m_move_window;
};

extern ECORE_API CHW		HW;
