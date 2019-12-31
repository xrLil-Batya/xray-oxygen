#include "stdafx.h"
#include "RadioactiveZone.h"
#include "level.h"
#include "xrMessages.h"
#include "../xrEngine/bone.h"
#include "Actor.h"
#include "game_base.h"
#include "Hit.h"
#include "../xrEngine/xr_collide_form.h"

CRadioactiveZone::CRadioactiveZone() 
{}

CRadioactiveZone::~CRadioactiveZone() 
{}

void CRadioactiveZone::Load(LPCSTR section) 
{
	inherited::Load(section);
}

bool  CRadioactiveZone::BlowoutState	()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();
	return result;
}

void CRadioactiveZone::Affect(SZoneObjectInfo* O) 
{
	float one				= 0.1f;
	float tg				= Device.fTimeGlobal;

	if(!O->object || O->f_time_affected+one > Device.fTimeGlobal) 
		return;

	clamp					(O->f_time_affected, tg-(one*3), tg);

	Fvector					pos; 
	XFORM().transform_tiny	(pos,CFORM()->getSphere().P);

	Fvector dir				={0,0,0}; 
	float power				= Power(O->object->Position().distance_to(pos),nearest_shape_radius(O));

	float impulse			= 0.0f;
	if(power < EPS)			
	{
		O->f_time_affected	= tg;
		return;
	}
	
	float send_power		= power*one;

	while(O->f_time_affected+one < tg)
	{
		CreateHit(O->object->ID(), ID(), dir,
						send_power, BI_NONE,
						Fvector().set(0.0f,0.0f,0.0f), impulse, m_eHitTypeBlowout);

		O->f_time_affected += one;
	}
}

void CRadioactiveZone::feel_touch_new					(CObject* O	)
{
	inherited::feel_touch_new(O);
};

BOOL CRadioactiveZone::feel_touch_contact(CObject* O)
{

	CActor* pAct = smart_cast<CActor*>(O);
	if (pAct)
	{ 
		if (!((CCF_Shape*)CFORM())->Contact(O))		
			return	FALSE;

		return pAct->feel_touch_on_contact(this);
	}
	else
		return FALSE;
}

void CRadioactiveZone::UpdateWorkload(u32 dt)
{
	inherited::UpdateWorkload(dt);
}

float CRadioactiveZone::nearest_shape_radius(SZoneObjectInfo* O)
{
	CCF_Shape* pSh = (CCF_Shape*)CFORM();

	if(pSh->Shapes().size()==1)
	{
		return Radius();
	}
	else
	{
		xr_vector<CCF_Shape::shape_def>& Shapes = pSh->Shapes();
		CCF_Shape::shape_def& s = Shapes[0];
		return s.data.sphere.R;
	}
}

