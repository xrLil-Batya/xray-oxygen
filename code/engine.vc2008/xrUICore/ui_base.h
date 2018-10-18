#pragma once
#include "linker.h"
#include "ui_defs.h"

class CUICursor;
class CUIGame;

class UI_API CDeviceResetNotifier :public pureDeviceReset
{
public:
					CDeviceResetNotifier	() { Device.seqDeviceReset.Add(this, REG_PRIORITY_NORMAL); };
	virtual			~CDeviceResetNotifier	() { Device.seqDeviceReset.Remove(this); };
	virtual void	OnDeviceReset			() {};
};

struct UI_API CFontManager :public pureDeviceReset
{
	// Hud font
	CGameFont* pFontMedium;
	CGameFont* pFontDI;

	xr_map<CGameFont*, const char*> FontVect;

					CFontManager		();
					~CFontManager		();

			void	InitializeFonts		();
			void	InitializeFont		(CGameFont*& F, LPCSTR section, u32 flags = 0);
			LPCSTR	GetFontTexName		(LPCSTR section);

			void	Render				();
	virtual void	OnDeviceReset		();
public:
	CGameFont*		GetFont				(const char* name);
};

class UI_API ui_core : public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	C2DFrustum		m_FrustumLIT;

	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	Fvector2		m_pp_scale_;
	Fvector2		m_scale_;
	Fvector2*		m_current_scale;

public:
	xr_stack<Frect>			m_Scissors;
	IUIRender::ePointType	m_currentPointType;

public:
							ui_core		();
							~ui_core	();

			CFontManager&	Font						() { return *m_pFontManager; }
			CUICursor&		GetUICursor					() { return *m_pUICursor; }

	IC		float			ClientToScreenScaledX		(float left) const { return left * m_current_scale->x; };
	IC		float			ClientToScreenScaledY		(float top)	 const { return top * m_current_scale->y; };

			void			ClientToScreenScaled		(Fvector2& dest, float left, float top)	const;
			void			ClientToScreenScaled		(Fvector2& src_and_dest)const;
			void			ClientToScreenScaledWidth	(float& src_and_dest)	const;
			void			ClientToScreenScaledHeight	(float& src_and_dest)	const;
			void			AlignPixel					(float& src_and_dest)	const;

	IC const C2DFrustum&	ScreenFrustum				()	const	{ return (m_bPostprocess) ? m_2DFrustumPP : m_2DFrustum; }
	IC		 C2DFrustum&	ScreenFrustumLIT			()			{ return m_FrustumLIT; }

			void			PushScissor					(const Frect& r, bool overlapped = false);
			void			PopScissor					();

			void			pp_start					();
			void			pp_stop						();
			void			RenderFont					();

	virtual void			OnDeviceReset				();
	static	bool			is_widescreen				();
	static	float			get_current_kx				();

	shared_str				get_xml_name				(LPCSTR fn);
};

extern UI_API CUICursor& GetUICursor();
extern UI_API ui_core &UI();
extern CUIGame* GameUI();
