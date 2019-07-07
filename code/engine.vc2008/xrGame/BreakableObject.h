// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "physicsshellholder.h"
#include "../xrphysics/icollisiondamagereceiver.h"

class	IPHStaticGeomShell;
class CBreakableObject: 
	public CPhysicsShellHolder,
	public ICollisionDamageReceiver
{
	using inherited =	CPhysicsShellHolder;
private:
		bool				b_resived_damage;
		float				m_max_frame_damage;
static	float				m_damage_threshold;
static	float				m_health_threshhold;
static	float				m_immunity_factor;
		Fvector				m_contact_damage_pos;
		Fvector				m_contact_damage_dir;

		float				fHealth;
		IPHStaticGeomShell	*m_pUnbrokenObject;
		IPhysicsShellEx		*m_Shell;
static	u32					m_remove_time;
		u32					m_break_time;
		bool				bRemoved;
//		Fbox				m_saved_box;
public:
					CBreakableObject	();
	virtual			~CBreakableObject	();

	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( CSE_Abstract* DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual	void	UpdateCL			();
	virtual BOOL	renderable_ShadowGenerate	( ) { return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit					(SHit* pHDS);

	virtual void	net_Export			(NET_Packet& P);
	virtual BOOL	UsedAI_Locations	();

private:
	void			Init				();
	void			CreateUnbroken		();
	void			CreateBroken		();
	void			DestroyUnbroken		();
	void			ActivateBroken		();
	void			Break				();
	void			ApplyExplosion		(const Fvector &dir,float impulse);
	void			CheckHitBreak		(float power,ALife::EHitType hit_type);
	void			ProcessDamage		();
	void			SendDestroy			();
	void			enable_notificate	();
private:
	virtual ICollisionDamageReceiver	*PHCollisionDamageReceiver	()								{return (this);}
	virtual		void					CollisionHit				( u16 source_id, u16 bone_id, float power, const Fvector &dir, Fvector &pos );

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CBreakableObject)
#undef script_type_list
#define script_type_list save_type_list(CBreakableObject)