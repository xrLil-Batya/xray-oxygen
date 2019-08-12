#include "stdafx.h"
#include "Actor.h"
#include "items/Torch.h"
#include "trade.h"
#include "../xrEngine/CameraBase.h"

#ifdef DEBUG
#	include "PHDebug.h"
#endif

#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "UIGame.h"
#include "inventory.h"
#include "level.h"
#include "..\xrEngine\xr_level_controller.h"
#include "UsableScriptObject.h"
#include "actorcondition.h"
#include "actor_input_handler.h"
#include "../xrEngine/string_table.h"
#include "../xrUICore/UIStatic.h"
#include "ui/UIActorMenu.h"
#include "ui/UIDragDropReferenceList.h"
#include "CharacterPhysicsSupport.h"
#include "InventoryBox.h"
#include "player_hud.h"
#include "../xrEngine/xr_input.h"
#include "flare.h"
#include "CustomDetector.h"
#include "clsid_game.h"
#include "hudmanager.h"
#include "items/Weapon.h"
#include "ZoneCampfire.h"
#include "../xrEngine/XR_IOConsole.h"
#include "script_callback_ex.h"
#include "searchlight.h"
#include "HudItem.h"
#include "../xrEngine/x_ray.h"
extern u32 hud_adj_mode;

void CActor::IR_OnKeyboardPress(u8 cmd)
{
	if (hud_adj_mode && pInput->iGetAsyncKeyState(VK_SHIFT))
	{
		if (pInput->iGetAsyncKeyState(VK_RETURN) || pInput->iGetAsyncKeyState(VK_BACK) ||
			pInput->iGetAsyncKeyState(VK_DELETE))
			g_player_hud->tune(Ivector().set(0, 0, 0));

			return;
	}

	if (IsTalking())	return;
	if (m_input_external_handler && !m_input_external_handler->authorized(cmd))	return;
	
	if (cmd == kWPN_FIRE && !pInput->iGetAsyncKeyState(VK_MENU) && !cam_active == eacFirstEye)
	{
		u16 slot = inventory().GetActiveSlot();
		if (inventory().ActiveItem() && (slot == INV_SLOT_3 || slot == INV_SLOT_2))
			mstate_wishful &= ~mcSprint;
	}

	if (!g_Alive()) return;

	if(m_holder && kUSE != cmd)
	{
		m_holder->OnKeyboardPress(cmd);
		if(m_holder->allowWeapon() && inventory().Action((u16)cmd, CMD_START)) return;
		return;
	}
	else if(inventory().Action((u16)cmd, CMD_START)) return;


	if(psActorFlags.test(AF_NO_CLIP))
	{
		NoClipFly(cmd);
		return;
	}

	if (pInput->iGetAsyncKeyState(VK_PRIOR))
	{
		CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());

		if (pWeapon)
			pWeapon->ChangeNextMark();
	}


	if (pInput->iGetAsyncKeyState(VK_NEXT))
	{
		CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());

		if (pWeapon)
			pWeapon->ChangePrevMark();
	}

	// Dev actions should work only if we on developer mode (-developer)
	if (cmd >= kDEV_ACTION1 && cmd < (kDEV_ACTION1 + 4))
	{
		if (GamePersistent().IsDeveloperMode())
			callback(GameObject::eOnActionPress)(cmd);
	}
	else
		callback(GameObject::eOnActionPress)(cmd);

	if (g_appLoaded)
	{
		switch (cmd)
		{
		case kCROUCH:		 if (psActorFlags.test(AF_CROUCH_TOGGLE) && !(mstate_real & (mcJump | mcFall))) mstate_wishful ^= mcCrouch; break;
		case kCAM_2:		 if (!psActorFlags.test(AF_HARDCORE)) cam_Set(eacLookAt); break;
		case kCAM_3:		 if (!psActorFlags.test(AF_HARDCORE)) cam_Set(eacFreeLook); break;

		case kQUICK_SAVE:	Console->Execute("save"); break;
		case kQUICK_LOAD:	Console->Execute("load"); break;
		case kUSE:			ActorUse(); break;
		case kDROP:			b_DropActivated = TRUE; f_DropPower = 0; break;
		case kNEXT_SLOT:	OnNextWeaponSlot(); break;
		case kPREV_SLOT:	OnPrevWeaponSlot(); break;
		case kNIGHT_VISION: SwitchNightVision(); break;

		case kFWD:		mstate_wishful |= mcFwd;	 m_movementWeight.y += 1.0f; 	break;
		case kBACK:		mstate_wishful |= mcBack;	 m_movementWeight.y += -1.0f;	break;
		case kL_STRAFE:	mstate_wishful |= mcLStrafe; m_movementWeight.x += -1.0f;	break;
		case kR_STRAFE:	mstate_wishful |= mcRStrafe; m_movementWeight.x += 1.0f; 	break;

		case kJUMP:			 mstate_wishful |= mcJump; break;
		case kSPRINT_TOGGLE: mstate_wishful ^= mcSprint; break;
		case kCAM_1:		 cam_Set(eacFirstEye); break;
		case kTORCH:
		{
			if (m_pProjWeLookingAt)
			{
				if (!m_pProjWeLookingAt->Get_light_active())
					m_pProjWeLookingAt->TurnOn();
				else
					m_pProjWeLookingAt->TurnOff();
			}
			else
				SwitchTorch();
			break;
		}

		case kDETECTOR:
		{
			PIItem det_active = inventory().ItemFromSlot(DETECTOR_SLOT);
			if (det_active)
			{
				CCustomDetector* det = smart_cast<CCustomDetector*>(det_active);
				det->ToggleDetector(g_player_hud->attached_item(0) != nullptr);
				return;
			}
		}break;

		case kQUICK_USE_1:
		case kQUICK_USE_2:
		case kQUICK_USE_3:
		case kQUICK_USE_4:
		{
			const shared_str& item_name = g_quick_use_slots[cmd - kQUICK_USE_1];
			if (item_name.size())
			{
				PIItem itm = inventory().GetAny(item_name.c_str());

				if (itm)
				{
					inventory().Eat(itm);

					SDrawStaticStruct* _s = GameUI()->AddCustomStatic("item_used", true);
					string1024					str;
					xr_strconcat(str, *CStringTable().translate("st_item_used"), ": ", itm->NameItem());
					_s->wnd()->TextItemControl()->SetText(str);

					GameUI()->ActorMenu().m_pQuickSlot->ReloadReferences(this);
				}
			}
		}break;
		}
	}
}

void CActor::IR_OnMouseWheel(int direction)
{
	if(hud_adj_mode)
	{
		g_player_hud->tune	(Ivector().set(0,0,direction));
		return;
	}

    if (inventory().Action(kWPN_ZOOM, (direction > 0) ? WeaponActionFlags::CMD_IN : WeaponActionFlags::CMD_OUT)) return;

	if (direction>0)
		OnNextWeaponSlot				();
	else
		OnPrevWeaponSlot				();
}

void CActor::IR_OnKeyboardRelease(u8 cmd)
{
	if(hud_adj_mode && pInput->iGetAsyncKeyState(VK_SHIFT))	return;
	if (Remote())	return;
	if (m_input_external_handler && !m_input_external_handler->authorized(cmd))	return;
	//g_loading_events
	if (g_Alive())	
	{
		// Dev actions should work only if we on developer mode (-developer)
		if (cmd >= kDEV_ACTION1 && cmd < (kDEV_ACTION1 + 4))
		{
			if (GamePersistent().IsDeveloperMode())
				callback(GameObject::eOnActionRelease)(cmd);
		}
		else
			callback(GameObject::eOnActionRelease)(cmd);

		if (m_holder)
		{
			m_holder->OnKeyboardRelease(cmd);

			if (m_holder->allowWeapon())
				inventory().Action((u16)cmd, CMD_STOP);

			return;
		}
		else inventory().Action((u16)cmd, CMD_STOP);

		if (g_appLoaded)
		{
			switch (cmd)
			{
			case kJUMP:		mstate_wishful &= ~mcJump; break;
			case kDROP:		if (GAME_PHASE_INPROGRESS == Game().Phase()) g_PerformDrop(); break;
			case kUSE:      m_bPickupMode = false;			break;

			case kFWD:		mstate_wishful &= ~mcFwd;	  m_movementWeight.y -= 1.0f;	break;
			case kBACK:		mstate_wishful &= ~mcBack;	  m_movementWeight.y += 1.0f;	break;
			case kL_STRAFE:	mstate_wishful &= ~mcLStrafe; m_movementWeight.x += 1.0f;	break;
			case kR_STRAFE:	mstate_wishful &= ~mcRStrafe; m_movementWeight.x -= 1.0f;	break;
			}
		}

	}
}

void CActor::IR_OnKeyboardHold(u8 cmd)
{
	if (hud_adj_mode && pInput->iGetAsyncKeyState(VK_SHIFT))
	{
		if (pInput->iGetAsyncKeyState(VK_UP))
			g_player_hud->tune(Ivector().set(0, -1, 0));
		if (pInput->iGetAsyncKeyState(VK_DOWN))
			g_player_hud->tune(Ivector().set(0, 1, 0));
		if (pInput->iGetAsyncKeyState(VK_LEFT))
			g_player_hud->tune(Ivector().set(-1, 0, 0));
		if (pInput->iGetAsyncKeyState(VK_RIGHT))
			g_player_hud->tune(Ivector().set(1, 0, 0));
		return;
	}

	if (!g_Alive()) return;
	if (m_input_external_handler && !m_input_external_handler->authorized(cmd))	return;
	if (IsTalking()) return;

	if (cmd >= kDEV_ACTION1 && cmd < (kDEV_ACTION1 + 4))
	{
		if (GamePersistent().IsDeveloperMode())
			callback(GameObject::eOnActionHold)(cmd);
	}
	else
		callback(GameObject::eOnActionHold)(cmd);

	if(m_holder)
	{
		m_holder->OnKeyboardHold(cmd);
		return;
	}

	if(psActorFlags.test(AF_NO_CLIP) && (cmd==kFWD || cmd==kBACK || cmd==kL_STRAFE || cmd==kR_STRAFE || cmd==kJUMP || cmd==kCROUCH))
	{
		NoClipFly(cmd);
		return;
	}

	float LookFactor = GetLookFactor();
	switch(cmd)
	{
	case kUP:
	case kDOWN:				cam_Active()->Move( (cmd==kUP) ? kDOWN : kUP, 0, LookFactor); break;
	case kCAM_ZOOM_IN:
	case kCAM_ZOOM_OUT:		cam_Active()->Move(cmd); break;
	case kLEFT:
	case kRIGHT:			if (eacFreeLook!=cam_active) cam_Active()->Move(cmd, 0, LookFactor); break;

	case kACCEL:	mstate_wishful |= mcAccel;									break;
	case kL_LOOKOUT:mstate_wishful |= mcLLookout;								break;
	case kR_LOOKOUT:mstate_wishful |= mcRLookout;								break;
	case kCROUCH:	{
						if (!psActorFlags.test(AF_CROUCH_TOGGLE) && !(mstate_real&(mcJump | mcFall)))
							mstate_wishful |= mcCrouch;
					}break;
	}
}

void CActor::IR_OnMouseMove(int dx, int dy)
{
	if (hud_adj_mode)
	{
		g_player_hud->tune(Ivector().set(dx, dy, 0));
		return;
	}

	PIItem iitem = inventory().ActiveItem();
	if (iitem && iitem->cast_hud_item())
		iitem->cast_hud_item()->ResetSubStateTime();

	if (m_holder)
	{
		m_holder->OnMouseMove(dx, dy);
		return;
	}

	float LookFactor = GetLookFactor();

	CCameraBase* C = cameras[cam_active];
	float scale = (C->f_fov / g_fov)*psMouseSens * psMouseSensScale / 50.f / LookFactor;
	if (dx) 
	{
		float d = float(dx)*scale;
		cam_Active()->Move((d < 0) ? kLEFT : kRIGHT, _abs(d));
	}
	if (dy) 
	{
		float d = ((psMouseInvert.test(1)) ? -1 : 1)*float(dy)*scale*3.f / 4.f;
		cam_Active()->Move((d > 0) ? kUP : kDOWN, _abs(d));
	}
}

bool CActor::use_Holder(CHolderCustom* holder)
{
	bool bUse = false;
	if(m_holder)
	{
		CGameObject* holderGO = smart_cast<CGameObject*>(m_holder);

		if (smart_cast<CCar*>(holderGO))
			bUse = use_Vehicle(nullptr);
		else if (holderGO->CLS_ID == CLSID_OBJECT_W_STATMGUN)
			bUse = use_MountedWeapon(nullptr);

		if(inventory().ActiveItem())
		{
			CHudItem* hi = smart_cast<CHudItem*>(inventory().ActiveItem());
			if(hi) hi->OnAnimationEnd(hi->GetState());
		}
	}
	else
	{
		bool bUse = false;
		CGameObject* holderGO = smart_cast<CGameObject*>(holder);
		if(smart_cast<CCar*>(holder))
			bUse = use_Vehicle(holder);

		if (holderGO->CLS_ID==CLSID_OBJECT_W_STATMGUN)
			bUse = use_MountedWeapon(holder);

		//used succesfully
		if(bUse)
		{
			// switch off torch...
			CAttachableItem *I = CAttachmentOwner::attachedItem(CLSID_DEVICE_TORCH);
			if (I)
			{
				CTorch* torch = smart_cast<CTorch*>(I);
				if (torch) torch->Switch(false);
			}
		}

		if(inventory().ActiveItem())
		{
			CHudItem* hi = smart_cast<CHudItem*>(inventory().ActiveItem());
			if(hi) hi->OnAnimationEnd(hi->GetState());
		}
	}
	return bUse;
}

void CActor::ActorUse()
{
	if (m_holder)
	{
		CGameObject* GO = smart_cast<CGameObject*>(m_holder);
		NET_Packet P;
		CGameObject::u_EventGen(P, GEG_PLAYER_DETACH_HOLDER, ID());
		P.w_u16(GO->ID());
		CGameObject::u_EventSend(P);
		return;
	}

	if (character_physics_support()->movement()->PHCapture())
		character_physics_support()->movement()->PHReleaseObject();

	if (m_pUsableObject && !m_pObjectWeLookingAt->cast_inventory_item())
		m_pUsableObject->use(this);

	if (m_pInvBoxWeLookingAt && m_pInvBoxWeLookingAt->nonscript_usable())
	{
		if (!m_pInvBoxWeLookingAt->closed())
			GameUI()->StartSearchBody(this, m_pInvBoxWeLookingAt);

		return;
	}
	if (m_pProjWeLookingAt)
	{
		m_pProjWeLookingAt->actor_use = !m_pProjWeLookingAt->actor_use && 
			Level().CurrentControlEntity()->Position().distance_to(m_pProjWeLookingAt->Position()) < 2.0f;
		return;
	}

	if (!m_pUsableObject || m_pUsableObject->nonscript_usable())
	{
		bool bCaptured = false;

		collide::rq_result& RQ = HUD().GetCurrentRayQuery();
		CPhysicsShellHolder* object = smart_cast<CPhysicsShellHolder*>(RQ.O);
		u16 element = BI_NONE;
		if (object)
		{
			element = (u16)RQ.element;

			if (Level().IR_GetKeyState(VK_SHIFT))
			{
				bool b_allow = !!pSettings->line_exist("ph_capture_visuals", object->cNameVisual());
				if (b_allow && !character_physics_support()->movement()->PHCapture())
				{
					character_physics_support()->movement()->PHCaptureObject(object, element);
					bCaptured = true;
				}
			}
			else if (smart_cast<CHolderCustom*>(object))
			{
				NET_Packet P;
				CGameObject::u_EventGen(P, GEG_PLAYER_ATTACH_HOLDER, ID());
				P.w_u16(object->ID());
				CGameObject::u_EventSend(P);
				return;
			}
			else if (m_pPersonWeLookingAt)
			{
				CEntityAlive* pEntityAliveWeLookingAt = smart_cast<CEntityAlive*>(m_pPersonWeLookingAt);
				VERIFY(pEntityAliveWeLookingAt);

				if (pEntityAliveWeLookingAt->g_Alive())
					TryToTalk();
				else
				{
					if (!m_pPersonWeLookingAt->deadbody_closed_status() && pEntityAliveWeLookingAt->AlreadyDie())
						GameUI()->StartSearchBody(this, m_pPersonWeLookingAt);
				}
			}
		}
	}
	// переключение костра при юзании
	if (m_CapmfireWeLookingAt)
	{
		m_CapmfireWeLookingAt->is_on() ? m_CapmfireWeLookingAt->turn_off_script() : m_CapmfireWeLookingAt->turn_on_script();
		return;
	}

	m_bPickupMode = true;
}

BOOL CActor::HUDview() const
{
	return IsFocused() && (cam_active == eacFirstEye) &&
		((!m_holder) || (m_holder && m_holder->allowWeapon() && m_holder->HUDView()));
}

static constexpr u16 SlotsToCheck [] = 
{
		KNIFE_SLOT		,		// 1
		INV_SLOT_2		,		// 2
		INV_SLOT_3		,		// 3
		GRENADE_SLOT	,		// 4
		ARTEFACT_SLOT	,		// 11
};

void CActor::OnNextWeaponSlot()
{
	u32 ActiveSlot = inventory().GetActiveSlot();
	if (ActiveSlot == NO_ACTIVE_SLOT) 
		ActiveSlot = inventory().GetPrevActiveSlot();

	if (ActiveSlot == NO_ACTIVE_SLOT) 
		ActiveSlot = KNIFE_SLOT;
	
	u32 NumSlotsToCheck = sizeof(SlotsToCheck) / sizeof(SlotsToCheck[0]);	
	
	u32 CurSlot = 0;
	while(CurSlot<NumSlotsToCheck)
	{
		if (SlotsToCheck[CurSlot] == ActiveSlot) break;
		CurSlot++;
	}

	if (CurSlot >= NumSlotsToCheck) 
		return;

	for (u32 i=CurSlot+1; i<NumSlotsToCheck; i++)
	{
		if (inventory().ItemFromSlot(SlotsToCheck[i]))
		{
            IR_OnKeyboardPress(kWPN_1 + i);
			return;
		}
	}
};

void CActor::OnPrevWeaponSlot()
{
	u32 ActiveSlot = inventory().GetActiveSlot();
	if (ActiveSlot == NO_ACTIVE_SLOT) 
		ActiveSlot = inventory().GetPrevActiveSlot();

	if (ActiveSlot == NO_ACTIVE_SLOT) 
		ActiveSlot = KNIFE_SLOT;

	u32 NumSlotsToCheck = sizeof(SlotsToCheck)/sizeof(SlotsToCheck[0]);	
	u32 CurSlot		= 0;

	for (; CurSlot < NumSlotsToCheck; CurSlot++)
	{
		if (SlotsToCheck[CurSlot] == ActiveSlot) break;
	}

	if (CurSlot >= NumSlotsToCheck) 
		CurSlot	= NumSlotsToCheck-1; //last in row

	if (CurSlot != 0)
	{
		for (u32 i = CurSlot - 1u; i >= 0; i--)
		{
			if (inventory().ItemFromSlot(SlotsToCheck[i]))
			{
				IR_OnKeyboardPress(kWPN_1 + i);
				return;
			}
		}
	}
};

float CActor::GetLookFactor()
{
	if (m_input_external_handler)
		return m_input_external_handler->mouse_scale_factor();

	float factor = 1.f;

	PIItem pItem = inventory().ActiveItem();
	if (pItem)
		factor = pItem->GetControlInertionFactor();

	VERIFY(!fis_zero(factor));

	return factor;
}

void CActor::set_input_external_handler(CActorInputHandler *handler)
{
	// clear state
	if (handler)
		mstate_wishful = 0;

	// release fire button
	if (handler)
		IR_OnKeyboardRelease(kWPN_FIRE);

	// set handler
	m_input_external_handler = handler;
}

#include "items/WeaponBinoculars.h"
#include "items/WeaponBinocularsVision.h"
#include "items/Helmet.h"
void CActor::SwitchNightVision()
{
	CWeapon* wpn1 = nullptr;
	CWeapon* wpn2 = nullptr;
	if(inventory().ItemFromSlot(INV_SLOT_2))
		wpn1 = smart_cast<CWeapon*>(inventory().ItemFromSlot(INV_SLOT_2));

	if(inventory().ItemFromSlot(INV_SLOT_3))
		wpn2 = smart_cast<CWeapon*>(inventory().ItemFromSlot(INV_SLOT_3));

	for (CAttachableItem* pAttachObj: CAttachmentOwner::attached_objects())
	{
		CTorch* torch = smart_cast<CTorch*>(pAttachObj);

		if (torch)
		{	
			if(wpn1 && wpn1->IsZoomed())
				return;

			if(wpn2 && wpn2->IsZoomed())
				return;

			torch->SwitchNightVision();
		}
	}
}

void CActor::SwitchTorch()
{
	for (CAttachableItem* pAttachObj : CAttachmentOwner::attached_objects())
	{
		CTorch* torch = smart_cast<CTorch*>(pAttachObj);
		if (torch)
		{		
			torch->Switch();
			return;
		}
	}
}

void CActor::SwitchTorchMode()
{ 
	xr_vector<CAttachableItem*> const& all = CAttachmentOwner::attached_objects();
	xr_vector<CAttachableItem*>::const_iterator it = all.begin();
	xr_vector<CAttachableItem*>::const_iterator it_e = all.end();
	for ( ; it != it_e; ++it )
	{
		CTorch* torch = smart_cast<CTorch*>(*it);
		if (torch)
		{		
			torch->SwitchTorchMode();
			return;
		}
	}
}

void CActor::NoClipFly(int cmd)
{
	Fvector cur_pos, right, left;
	cur_pos.set(0,0,0);
	float scale = 0.55f;
	if(pInput->iGetAsyncKeyState(VK_SHIFT))
		scale = 0.25f;
	else if(pInput->iGetAsyncKeyState(VK_X))
		scale = 1.0f;
	else if(pInput->iGetAsyncKeyState(VK_MENU))
		scale = 2.0f;//LALT
	else if(pInput->iGetAsyncKeyState(VK_TAB))
		scale = 5.0f;

	switch(cmd)
	{
	case kJUMP:		cur_pos.y += 0.12f; break;
	case kCROUCH:	cur_pos.y -= 0.12f; break;
	case kFWD:		cur_pos.mad(cam_Active()->vDirection, scale / 2.0f); break;
	case kBACK:		cur_pos.mad(cam_Active()->vDirection, -scale / 2.0f); break;
	case kL_STRAFE: left.crossproduct(cam_Active()->vNormal, cam_Active()->vDirection);  cur_pos.mad(left, -scale / 2.0f); break;
	case kR_STRAFE: right.crossproduct(cam_Active()->vNormal, cam_Active()->vDirection); cur_pos.mad(right, scale / 2.0f); break;
	case kCAM_1:	cam_Set(eacFirstEye); break;
	case kCAM_2:	cam_Set(eacLookAt);	 break;
	case kCAM_3:	cam_Set(eacFreeLook); break;

	case kNIGHT_VISION: SwitchNightVision(); break;
	case kUSE:			ActorUse(); break;

	case kTORCH:
		if (m_pProjWeLookingAt)
		{
			if (!m_pProjWeLookingAt->Get_light_active())
				m_pProjWeLookingAt->TurnOn();
			else
				m_pProjWeLookingAt->TurnOff();
		}
		else SwitchTorch();
		break;
	case kDETECTOR:
		{
			PIItem det_active = inventory().ItemFromSlot(DETECTOR_SLOT);
			if(det_active)
			{
				CCustomDetector* det = smart_cast<CCustomDetector*>(det_active);
				det->ToggleDetector(g_player_hud->attached_item(0)!=nullptr);
				return;
			}
		}
		break;
	}
	cur_pos.mul(scale);
	Position().add(cur_pos);
	XFORM().translate_add(cur_pos);
	character_physics_support()->movement()->SetPosition(Position());
}


void CActor::IR_OnThumbstickChanged(GamepadThumbstickType type, const Fvector2& position)
{
	if (type == GamepadThumbstickType::Left)
	{
		// movement!
		m_movementWeight = position;
		m_movementWeight.y = -position.y;
		if (position.y > 0.1f)
		{
			mstate_wishful |= mcFwd;
		}

		if (position.y < -0.1f)
		{
			mstate_wishful |= mcBack;
		}

		if (position.x < -0.1f)
		{
			mstate_wishful |= mcLStrafe;
		}

		if (position.x > 0.1f)
		{
			mstate_wishful |= mcRStrafe;
		}
	}
	else if (type == GamepadThumbstickType::Right)
	{
		// camera!

		const float scale = 45.0f;
		float dX = position.x * scale;
		float dY = position.y * scale;
		m_cameraMoveWeight.set(dX, dY);
	}
}


void CActor::ResetMovementWeight()
{
	m_movementWeight.x = 0.0f;
	m_movementWeight.y = 0.0f;
	int forwardKey = get_action_dik(kFWD);
	int backKey = get_action_dik(kBACK);
	int LeftStrafeKey = get_action_dik(kL_STRAFE);
	int RightStrafeKey = get_action_dik(kR_STRAFE);
	if (pInput->iGetAsyncBtnState(forwardKey))
	{
		m_movementWeight.y += 1.0f;
	}

	if (pInput->iGetAsyncBtnState(backKey))
	{
		m_movementWeight.y += -1.0f;
	}

	if (pInput->iGetAsyncBtnState(LeftStrafeKey))
	{
		m_movementWeight.x += -1.0f;
	}

	if (pInput->iGetAsyncBtnState(RightStrafeKey))
	{
		m_movementWeight.x += 1.0f;
	}
}