// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "gameobject.h"
#include "physicsshellholder.h"
#include "PHSkeleton.h"
#include "hit_immunity.h"
#include "../xrScripts/export/script_export_space.h"
// refs
class CLAItem;
class IPhysicsElementEx;
class CSE_ALifeObjectHangingLamp;
class CPHElement;

class CHangingLamp: public CPhysicsShellHolder, public CPHSkeleton, public CHitImmunity
{
	using inherited =	CPhysicsShellHolder;
private:
	u16				light_bone;
	u16				ambient_bone;

	ref_light		light_render;
	ref_light		light_ambient;
	CLAItem*		lanim;
	float			ambient_power;
	BOOL			m_bState;
	
	ref_glow		glow_render;
	
	float			fHealth;
	float			fBrightness;
	void			CreateBody		(CSE_ALifeObjectHangingLamp	*lamp);
	void			Init();
	void			RespawnInit		();
	bool			Alive			(){return fHealth>0.f;}


public:
					CHangingLamp	();
	virtual			~CHangingLamp	();
	void			TurnOn			();
	void			TurnOff			();
	void			Load			( LPCSTR section) override;
	BOOL			net_Spawn		( CSE_Abstract* DC) override;
	void			net_Destroy		() override;
	void			shedule_Update	( u32 dt) override;							// Called by scheduler
	void			UpdateCL		( ) override;								// Called each frame, so no need for dt


	void			SpawnInitPhysics	(CSE_Abstract	*D) override;
	CPhysicsShellHolder*	PPhysicsShellHolder	() override	{return PhysicsShellHolder();};
	void			CopySpawnInit		() override;
	void			net_Save			(NET_Packet& P) override;
	BOOL			net_SaveRelevant	() override;
	void			save				(NET_Packet &output_packet) override;
	void			load				(IReader &input_packet) override;

	BOOL			renderable_ShadowGenerate	( ) override { return TRUE;	}
	BOOL			renderable_ShadowReceive	( ) override { return TRUE;	}
	
	void			Hit				(SHit* pHDS) override;
	void			net_Export		(NET_Packet& P) override;
	BOOL			UsedAI_Locations() override;

	void			Center			(Fvector& C)	const noexcept override;
	float			Radius			()				const noexcept override;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CHangingLamp)
#undef script_type_list
#define script_type_list save_type_list(CHangingLamp)