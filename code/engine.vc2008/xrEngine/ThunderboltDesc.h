#pragma once

class ENGINE_API CLAItem;

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/LensFlareRender.h"
#include "../Include/xrRender/ThunderboltDescRender.h"
#include "../Include/xrRender/ThunderboltRender.h"

class ENGINE_API CThunderboltDesc
{
public:
	FactoryPtr<IThunderboltDescRender> m_pRender;
    ref_sound snd;

	struct SFlare
	{
    	float					fOpacity;
	    Fvector2				fRadius;
        shared_str				texture;
        shared_str				shader;
		FactoryPtr<IFlareRender>m_pFlare;
    	SFlare()
		{ 
			fOpacity = 0.0f;
			fRadius.set(0.0f, 0.0f);
		}
	};
    SFlare*						m_GradientTop;
    SFlare*						m_GradientCenter;
    shared_str					name;
	CLAItem*					color_anim;
public:
								CThunderboltDesc		();
	INGAME_EDITOR_VIRTUAL	    ~CThunderboltDesc		();
						  void	Load					(CInifile& pIni, shared_str const& sect);
	INGAME_EDITOR_VIRTUAL void	CreateTopGradient		(CInifile& pIni, shared_str const& sect);
	INGAME_EDITOR_VIRTUAL void	CreateCenterGradient	(CInifile& pIni, shared_str const& sect);
};
