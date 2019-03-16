#pragma once

#include "../xrCDB/xr_collide_defs.h"

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/LensFlareRender.h"

#include "LensFlareDescriptor.h"

class ENGINE_API CInifile;
class ENGINE_API CEnvironment;

using LensFlareDescVec = xr_vector<CLensFlareDescriptor*>;

class ENGINE_API CLensFlare
{
	friend class dxLensFlareRender;
public:
	enum
	{
		MAX_RAYS	= 5
	};

private:
	collide::rq_results	r_dest;
#ifndef _EDITOR
	collide::ray_cache	m_ray_cache[MAX_RAYS];
#endif

protected:
	float				fBlend;
	u32					dwFrame;

	Fvector				vSunDir;
	Fvector				vecLight;
	Fvector				vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL				bRender;

	// variable
    Fcolor				LightColor;
	float				fGradientValue;

	FactoryPtr<ILensFlareRender>	m_pRender;
	//ref_geom			hGeom;

    LensFlareDescVec	m_Palette;
	CLensFlareDescriptor* m_Current;

public:
    enum LFState{
        lfsNone,
        lfsIdle,
    	lfsHide,
        lfsShow,
    };

protected:
    LFState				m_State;
    float				m_StateBlend;

public:
						CLensFlare		();
	virtual				~CLensFlare		();

	void				OnFrame			(shared_str id);
    void __fastcall		Render			(bool bSun, bool bFlares, bool bGradient);
	void 				OnDeviceCreate	();         
	void 				OnDeviceDestroy	();

    shared_str			AppendDef		(CEnvironment& environment, CInifile* pIni, LPCSTR sect);

	void				Invalidate		(){m_State=lfsNone;}
};
