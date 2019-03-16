#include "stdafx.h"
#include "mosquitobald.h"
#include "../xrParticles/psystem.h"
#include "../xrParticles/ParticlesObject.h"
#include "level.h"
#include "physicsshellholder.h"
#include "../xrengine/xr_collide_form.h"

CMosquitoBald::CMosquitoBald()
{
	m_fHitImpulseScale = 1.f;
	m_bLastBlowoutUpdate = false;
}

CMosquitoBald::~CMosquitoBald() {}

void CMosquitoBald::Load(LPCSTR section) 
{
	inherited::Load(section);
}

bool CMosquitoBald::BlowoutState()
{
	bool result = inherited::BlowoutState();

	if (!result)
	{
		m_bLastBlowoutUpdate = false;
	}
	else if (!m_bLastBlowoutUpdate)
	{
		m_bLastBlowoutUpdate = true;
	}

	UpdateBlowout();

	return result;
}

void CMosquitoBald::Affect(SZoneObjectInfo* O)
{
	CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>(O->object);
	if (!pGameObject) 
		return;

	if (O->zone_ignore) 
		return;

	Fvector P;
	XFORM().transform_tiny(P, CFORM()->getSphere().P);

	Fvector hit_dir;
	hit_dir.set(::Random.randF(-.5f, .5f), ::Random.randF(.0f, 1.f), ::Random.randF(-.5f, .5f));
	hit_dir.normalize();

	Fvector position_in_bone_space;

	VERIFY(!pGameObject->getDestroy());

	float dist = pGameObject->Position().distance_to(P) - pGameObject->Radius();
	float power = Power(dist > 0.f ? dist : 0.f, Radius());
	float impulse = m_fHitImpulseScale * power*pGameObject->GetMass();

	if (power > 0.01f)
	{
		position_in_bone_space.set(0.f, 0.f, 0.f);
		CreateHit(pGameObject->ID(), ID(), hit_dir, power, 0, position_in_bone_space, impulse, m_eHitTypeBlowout);
		PlayHitParticles(pGameObject);
	}
}

void CMosquitoBald::UpdateSecondaryHit()
{
	if (m_dwAffectFrameNum == Device.dwFrame)
		return;

	m_dwAffectFrameNum = Device.dwFrame;
	if (Device.dwPrecacheFrame)
		return;

	for (auto it : m_ObjectInfoMap)
	{
		if (!it.object->getDestroy())
		{
			CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>((&it)->object);
			if (!pGameObject) 
				return;

			if ((&it)->zone_ignore) 
				return;

			Fvector P;
			XFORM().transform_tiny(P, CFORM()->getSphere().P);

			Fvector hit_dir;
			hit_dir.set(::Random.randF(-.5f, .5f), ::Random.randF(.0f, 1.f), ::Random.randF(-.5f, .5f));
			hit_dir.normalize();

			Fvector position_in_bone_space;

			VERIFY(!pGameObject->getDestroy());

			float dist = pGameObject->Position().distance_to(P) - pGameObject->Radius();
			float power = m_fSecondaryHitPower * RelativePower(dist > 0.f ? dist : 0.f, Radius());
			if (power < 0.0f)
				return;

			float impulse = m_fHitImpulseScale * power*pGameObject->GetMass();
			position_in_bone_space.set(0.f, 0.f, 0.f);
			CreateHit(pGameObject->ID(), ID(), hit_dir, power, 0, position_in_bone_space, impulse, m_eHitTypeBlowout);
		}
	}
}

#include "ZoneCampfire.h"
#include "TorridZone.h"


#include "luabind/luabind.hpp"
using namespace luabind;


#pragma optimize("gyts",on)
void CMosquitoBald::script_register	(lua_State *L)
{
	module(L)
	[	
		class_<CTorridZone,CGameObject>("CTorridZone")
			.def(constructor<>()),
		class_<CMosquitoBald,CGameObject>("CMosquitoBald")
			.def(constructor<>()),
		class_<CZoneCampfire,CGameObject>("CZoneCampfire")
			.def(constructor<>())
			.def("turn_on",				&CZoneCampfire::turn_on_script)
			.def("turn_off",			&CZoneCampfire::turn_off_script)
			.def("is_on",				&CZoneCampfire::is_on)
	];
}
