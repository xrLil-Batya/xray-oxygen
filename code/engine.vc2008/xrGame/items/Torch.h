#pragma once

#include "inventory_item_object.h"
#include "hudsound.h"
#include "../xrScripts/export/script_export_space.h"

class CLAItem;
class CNightVisionEffector;

class GAME_API CTorch : public CInventoryItemObject 
{
    using inherited = CInventoryItemObject;

protected:
	float			fBrightness;
	CLAItem*		lanim;

	u16				guid_bone;
	shared_str		light_trace_bone;

	float			m_delta_h;
	Fvector2		m_prev_hp;
	bool			m_switched_on;

    //#REFACTOR: Not used anymore, input was cutout long ago
	int				torch_mode;

	float           range;
	float           range_o;
	float           range_2;
	float           range_o_2;
	float           glow_radius;
	float           glow_radius_2;
	ref_light		light_render;
	ref_light		light_omni;
	ref_glow		glow_render;
	Fvector			m_focus;

public:
					CTorch					();
	virtual			~CTorch					();

	virtual void	Load					(LPCSTR section);
	virtual BOOL	net_Spawn				(CSE_Abstract* DC);
	virtual void	net_Destroy				();
	virtual void	net_Export				(NET_Packet& P);				// export to server

	virtual void	OnH_A_Chield			();
	virtual void	OnH_B_Independent		(bool just_before_destroy);

	virtual void	UpdateCL				();

			void	SwitchTorchMode			();
			void	SwitchSoundPlay			();
			void	Switch					();
			void	Switch					(bool light_on);
			bool	torch_active			() const;

	virtual bool	can_be_attached			() const;

	//CAttachableItem
	virtual	void	enable					(bool value);
 
public:
			void	SwitchNightVision		();
			void	SwitchNightVision		(bool light_on, bool use_sounds=true);

			bool	GetNightVisionStatus	() { return m_bNightVisionOn; }
CNightVisionEffector* GetNightVision		() { return m_night_vision; }
protected:
	bool					m_bNightVisionEnabled;
	bool					m_bNightVisionOn;

	CNightVisionEffector*	m_night_vision;
	HUD_SOUND_COLLECTION	m_sounds;

	enum EStats{
		eTorchActive				= (1<<0),
		eNightVisionActive			= (1<<1),
		eAttached					= (1<<2)
	};

public:

	virtual bool			use_parent_ai_locations	() const
	{
		return				(!H_Parent());
	}
	virtual void	create_physic_shell		();
	virtual void	activate_physic_shell	();
	virtual void	setup_physic_shell		();

	virtual void	afterDetach				();
	virtual void	renderable_Render		();
	ref_sound 		m_switch_sound;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

class CNightVisionEffector
{
	CActor*					m_pActor;
	HUD_SOUND_COLLECTION	m_sounds;
public:
	enum EPlaySounds{
		eStartSound	= 0,
		eStopSound,
		eIdleSound,
		eBrokeSound
	};
				CNightVisionEffector(const shared_str& sect);
	void		Start		(const shared_str& sect, CActor* pA, bool play_sound=true);
	void		Stop		(const float factor, bool play_sound=true);
	bool		IsActive	();
	void		OnDisabled	(CActor* pA, bool play_sound=true);
	void		PlaySounds	(EPlaySounds which);
};

add_to_type_list(CTorch)
#undef script_type_list
#define script_type_list save_type_list(CTorch)
