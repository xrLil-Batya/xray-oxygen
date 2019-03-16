#pragma once

class ENGINE_API IRender_DetailModel;
class ENGINE_API CLAItem;

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/LensFlareRender.h"
#include "../Include/xrRender/ThunderboltDescRender.h"
#include "../Include/xrRender/ThunderboltRender.h"

class CThunderboltDesc;
class CThunderboltCollection;
class CEnvironment;

#define THUNDERBOLT_CACHE_SIZE 8

class ENGINE_API CEffect_Thunderbolt
{
	friend class dxThunderboltRender;
protected:
    using CollectionVec = xr_vector<CThunderboltCollection*>;
	CollectionVec				collection;
	CThunderboltDesc*			current;
private:
    Fmatrix				  		current_xform;
	Fvector3					current_direction;

	FactoryPtr<IThunderboltRender>	m_pRender;

    // States
	enum EState
	{
        stIdle,
		stWorking
	};
	EState						state;

    Fvector						lightning_center;
    float						lightning_size;
    float						lightning_phase;

    float						life_time;
    float						current_time;
    float						next_lightning_time;
	bool						bEnabled;

    // params
//	Fvector2					p_var_alt;
//	float						p_var_long;
//	float						p_min_dist;
//	float						p_tilt;
//	float						p_second_prop;
//	float						p_sky_color;
//	float						p_sun_color;
//	float						p_fog_color;
private:
    static bool					RayPick				(const Fvector& s, const Fvector& d, float& range);
    void						Bolt				(shared_str id, float period, float life_time);
public:                     
								CEffect_Thunderbolt	(); 
								~CEffect_Thunderbolt();

	void						OnFrame				(shared_str id,float period, float duration);
	void						Render				();

	shared_str 					AppendDef			(CEnvironment& environment, CInifile* pIni, CInifile* thunderbolts, LPCSTR sect);
};
