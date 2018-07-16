#include "stdafx.h"
#include "UIActorMenu.h"
#include "UIActorStateInfo.h"
#include "../actor.h"
#include "../UiGame.h"
#include "../inventory.h"
#include "../inventory_item.h"
#include "../InventoryBox.h"
#include "object_broker.h"
#include "../ai/monsters/BaseMonster/base_monster.h"
#include "UIInventoryUtilities.h"
#include "game_cl_base.h"

#include "../Weapon.h"
#include "../WeaponMagazinedWGrenade.h"
#include "../WeaponAmmo.h"
#include "../Silencer.h"
#include "../Scope.h"
#include "../GrenadeLauncher.h"
#include "../trade_parameters.h"
#include "../ActorHelmet.h"
#include "../CustomOutfit.h"
#include "../CustomDetector.h"
#include "../eatable_item.h"

#include "UIProgressBar.h"
#include "UICursor.h"
#include "UICellItem.h"
#include "UICharacterInfo.h"
#include "UIItemInfo.h"
#include "UIDragDropListEx.h"
#include "UIDragDropReferenceList.h"
#include "UIInventoryUpgradeWnd.h"
#include "UI3tButton.h"
#include "UIBtnHint.h"
#include "UIMessageBoxEx.h"
#include "UIPropertiesBox.h"
#include "UIMainIngameWnd.h"
#include "../Trade.h"
#include "../ActorRuck.h"

#include "../../FrayBuildConfig.hpp"

void CUIActorMenu::SetActor(CInventoryOwner* io)
{
	R_ASSERT(!IsShown());
	m_last_time = Device.dwTimeGlobal;
	m_pActorInvOwner = io;

	if (io)
		m_ActorCharacterInfo->InitCharacter(m_pActorInvOwner->object_id());
	else
		m_ActorCharacterInfo->ClearInfo();
}

void CUIActorMenu::SetPartner(CInventoryOwner* io)
{
	R_ASSERT			(!IsShown());
	m_pPartnerInvOwner	= io;
	if (m_pPartnerInvOwner)
	{
		CBaseMonster* monster = smart_cast<CBaseMonster*>(m_pPartnerInvOwner);
		if (monster || m_pPartnerInvOwner->use_simplified_visual())
		{
			m_PartnerCharacterInfo->ClearInfo();
			if (monster)
			{
				shared_str monster_tex_name = pSettings->r_string(monster->cNameSect(), "icon");
				m_PartnerCharacterInfo->InitMonsterCharacter(monster_tex_name);
			}
		}
		else
			m_PartnerCharacterInfo->InitCharacter(m_pPartnerInvOwner->object_id());
		SetInvBox(nullptr);
	}
	else m_PartnerCharacterInfo->ClearInfo();
}

void CUIActorMenu::SetInvBox(CInventoryBox* box)
{
	R_ASSERT			(!IsShown());
	m_pInvBox = box;
	if ( box )
	{
		m_pInvBox->set_in_use( true );
		SetPartner( NULL );
	}
}

void CUIActorMenu::SetMenuMode(EMenuMode mode)
{
	SetCurrentItem(nullptr);
	m_hint_wnd->set_text(nullptr);

	if (mode != m_currMenuMode)
	{
		m_ActorCharacterInfo->SetActorIcon();

		switch (m_currMenuMode)
		{
		case mmUndefined: break;
		case mmInventory: DeInitInventoryMode(); break;
		case mmTrade: DeInitTradeMode(); break;
		case mmUpgrade: DeInitUpgradeMode(); break;
		case mmDeadBodySearch: DeInitDeadBodySearchMode(); break;
		default: R_ASSERT(0); break;
		}

        //Hide zone map, show this in ResetMode()
		GameUI()->UIMainIngameWnd->ShowZoneMap(false);

		m_currMenuMode = mode;
		switch (m_currMenuMode)
		{
		case mmUndefined: ResetMode(); break;
		case mmInventory: InitInventoryMode(); break;
		case mmTrade: InitTradeMode(); break;
		case mmUpgrade: InitUpgradeMode(); break;
		case mmDeadBodySearch: InitDeadBodySearchMode(); break;
		default: R_ASSERT(0); break;
		}
		UpdateConditionProgressBars();
		CurModeToScript();
	}

	if (m_pActorInvOwner)
	{
		UpdateOutfit();
		UpdateActor();
	}
	UpdateButtonsLayout();
}

void CUIActorMenu::PlaySnd(eActorMenuSndAction a)
{
	if (sounds[a]._handle())
        sounds[a].play					(NULL, sm_2D);
}

void CUIActorMenu::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent		(pWnd, msg, pData);
}

void CUIActorMenu::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		SetMenuMode(m_currMenuMode);
		PlaySnd(eSndOpen);
		m_ActorStateInfo->UpdateActorInfo(m_pActorInvOwner);
	}
	else
	{
		PlaySnd(eSndClose);
		SetMenuMode(mmUndefined);
		Actor()->RepackAmmo();
	}
	m_ActorStateInfo->Show(status);
}

void CUIActorMenu::Draw()
{
	if (psHUD_Flags.is(HUD_DRAW)) 
	{
	    GameUI()->UIMainIngameWnd->DrawZoneMap();
	    m_hint_wnd->Draw();
		GameUI()->UIMainIngameWnd->DrawMainIndicatorsForInventory();
	}

	inherited::Draw();
	m_ItemInfo->Draw();
}

void CUIActorMenu::Update()
{
	m_last_time = Device.dwTimeGlobal;
	m_ActorStateInfo->UpdateActorInfo(m_pActorInvOwner);

	switch (m_currMenuMode)
	{
		case mmUndefined: break;
		case mmInventory: GameUI()->UIMainIngameWnd->UpdateZoneMap(); break;
		case mmDeadBodySearch: CheckDistance(); break;

		case mmTrade:
		{
			if (m_pPartnerInvOwner->inventory().ModifyFrame() != m_trade_partner_inventory_state)
				InitPartnerInventoryContents();
			CheckDistance();
			break;
		}

		case mmUpgrade:
		{
			UpdateUpgradeItem();
			CheckDistance();
			break;
		}

		default: R_ASSERT(0); break;
	}

	inherited::Update();
	m_ItemInfo->Update();
	m_hint_wnd->Update();
}

bool CUIActorMenu::StopAnyMove()  // true = актёр не идёт при открытом меню
{
	return (m_currMenuMode != mmInventory);
}

void CUIActorMenu::CheckDistance()
{
	CGameObject* pActorGO	= smart_cast<CGameObject*>(m_pActorInvOwner);
	CGameObject* pPartnerGO	= smart_cast<CGameObject*>(m_pPartnerInvOwner);
	CGameObject* pBoxGO		= smart_cast<CGameObject*>(m_pInvBox);
	VERIFY( pActorGO && (pPartnerGO || pBoxGO) );

	if (pPartnerGO)
	{
		if ((pActorGO->Position().distance_to(pPartnerGO->Position()) > 3.0f) && !m_pPartnerInvOwner->NeedOsoznanieMode())
		{
			g_btnHint->Discard();
			HideDialog();
		}
	}
	else //pBoxGO
	{
		VERIFY(pBoxGO);
		if (pActorGO->Position().distance_to(pBoxGO->Position()) > 3.0f)
		{
			g_btnHint->Discard();
			HideDialog();
		}
	}
}

EDDListType CUIActorMenu::GetListType(CUIDragDropListEx* l)
{
	if (l == m_pInventoryBagList)			return iActorBag;
	if (l == m_pInventoryBeltList)			return iActorBelt;

	if (l == m_pInventoryAutomaticList)	    return iActorSlot;
	if (l == m_pInventoryPistolList)		return iActorSlot;
	if (l == m_pInventoryOutfitList)		return iActorSlot;
	if (l == m_pInventoryHelmetList)		return iActorSlot;
	if (l == m_pInventoryDetectorList)		return iActorSlot;
	if (l == m_pInventoryRuckList)			return iActorSlot;

    if (l == m_pInventoryKnifeList)         return iActorSlot;
    if (l == m_pInventoryBinocularList)     return iActorSlot;
	
	if (l == m_pTradeActorBagList)			return iActorBag;
	if (l == m_pTradeActorList)			    return iActorTrade;
	if (l == m_pTradePartnerBagList)		return iPartnerTradeBag;
	if (l == m_pTradePartnerList)			return iPartnerTrade;
	if (l == m_pDeadBodyBagList)			return iDeadBodyBag;

	if(l == m_pQuickSlot)					return iQuickSlot;
	if(l == m_pTrashList)					return iTrashSlot;

	R_ASSERT(0);
	
	return iInvalid;
}

CUIDragDropListEx* CUIActorMenu::GetListByType(EDDListType t)
{
	switch (t)
	{
	case iActorBag: return (m_currMenuMode == mmTrade) ? m_pTradeActorBagList : m_pInventoryBagList; break;
	case iDeadBodyBag: return m_pDeadBodyBagList;
    case iActorBelt: return m_pInventoryBeltList;
	default: R_ASSERT("invalid call"); return m_pDeadBodyBagList;
	}
}

CUICellItem* CUIActorMenu::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUIActorMenu::CurrentIItem()
{
	return	(m_pCurrentCellItem)? (PIItem)m_pCurrentCellItem->m_pData : NULL;
}

void CUIActorMenu::SetCurrentItem(CUICellItem* itm)
{
	m_repair_mode = false;
	m_pCurrentCellItem = itm;
	if ( !itm )
	{
		InfoCurItem( NULL );
	}
	TryHidePropertiesBox();

	if ( m_currMenuMode == mmUpgrade )
	{
		SetupUpgradeItem();
	}
}

void CUIActorMenu::InfoCurItem(CUICellItem* cell_item)
{
	if (!cell_item)
	{
		m_ItemInfo->InitItem(NULL);
		return;
	}
	PIItem current_item = (PIItem)cell_item->m_pData;

	PIItem compare_item = NULL;
	u16    compare_slot = current_item->BaseSlot();
	if (compare_slot != NO_ACTIVE_SLOT)
	{
		compare_item = m_pActorInvOwner->inventory().ItemFromSlot(compare_slot);
	}

	if (GetMenuMode() == mmTrade)
	{
		CInventoryOwner* item_owner = smart_cast<CInventoryOwner*>(current_item->m_pInventory->GetOwner());
		u32 item_price = u32(-1);
		if (item_owner && item_owner == m_pActorInvOwner)
			item_price = m_partner_trade->GetItemPrice(current_item, true);
		else
			item_price = m_partner_trade->GetItemPrice(current_item, false);

		CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(current_item);
		if (ammo)
		{
			for (u32 j = 0; j < cell_item->ChildsCount(); ++j)
			{
				u32 tmp_price = 0;
				PIItem jitem = (PIItem)cell_item->Child(j)->m_pData;
				CInventoryOwner* ammo_owner = smart_cast<CInventoryOwner*>(jitem->m_pInventory->GetOwner());
				if (ammo_owner && ammo_owner == m_pActorInvOwner)
					tmp_price = m_partner_trade->GetItemPrice(jitem, true);
				else
					tmp_price = m_partner_trade->GetItemPrice(jitem, false);

				item_price += tmp_price;
			}
		}

		if (!current_item->CanTrade() ||
			(!m_pPartnerInvOwner->trade_parameters().enabled(CTradeParameters::action_buy(0),
				current_item->object().cNameSect()) &&
				item_owner && item_owner == m_pActorInvOwner)
			)
			m_ItemInfo->InitItem(cell_item, compare_item, u32(-1), "st_no_trade_tip_1");
		else if (current_item->GetCondition() < m_pPartnerInvOwner->trade_parameters().buy_item_condition_factor)
			m_ItemInfo->InitItem(cell_item, compare_item, u32(-1), "st_no_trade_tip_2");
		else
			m_ItemInfo->InitItem(cell_item, compare_item, item_price);
	}
	else
		m_ItemInfo->InitItem(cell_item, compare_item, u32(-1));

	float dx_pos = GetWndRect().left;
	fit_in_rect(m_ItemInfo, Frect().set(0.0f, 0.0f, UI_BASE_WIDTH - dx_pos, UI_BASE_HEIGHT), 10.0f, dx_pos);
}

void CUIActorMenu::UpdateItemsPlace()
{
	switch (m_currMenuMode)
	{
	case mmUndefined: break;
	case mmInventory: break;
	case mmTrade: UpdatePrices(); break;
	case mmUpgrade: SetupUpgradeItem(); break;
	case mmDeadBodySearch: UpdateDeadBodyBag(); break;
	default: R_ASSERT(0); break;
	}

	if (m_pActorInvOwner)
	{
		UpdateOutfit();
		UpdateActor();
	}
}
// ================================================================

void CUIActorMenu::clear_highlight_lists()
{
	m_InvSlot2Highlight->Show(false);
	m_InvSlot3Highlight->Show(false);
	m_HelmetSlotHighlight->Show(false);
	m_OutfitSlotHighlight->Show(false);
	m_DetectorSlotHighlight->Show(false);

    if (g_extraFeatures.is(GAME_EXTRA_RUCK))
    {
        m_RuckSlotHighlight->Show(false);
    }

    m_KnifeSlotHighlight->Show(false);
    m_BinocularSlotHighlight->Show(false);

	
	for(u8 i=0; i<4; i++)
		m_QuickSlotsHighlight[i]->Show(false);
	for(u8 i=0; i<e_af_count; i++)
		m_ArtefactSlotsHighlight[i]->Show(false);

	m_pInventoryBagList->clear_select_armament();

	switch ( m_currMenuMode )
	{
	case mmUndefined:
		break;
	case mmInventory:
		break;
	case mmTrade:
		m_pTradeActorBagList->clear_select_armament();
		m_pTradeActorList->clear_select_armament();
		m_pTradePartnerBagList->clear_select_armament();
		m_pTradePartnerList->clear_select_armament();
		break;
	case mmUpgrade:
		break;
	case mmDeadBodySearch:
		m_pDeadBodyBagList->clear_select_armament();
		break;
	}
	m_highlight_clear = true;
}

void CUIActorMenu::highlight_item_slot(CUICellItem* cell_item)
{
	PIItem item = (PIItem)cell_item->m_pData;
	if(!item)
		return;

	if(CUIDragDropListEx::m_drag_item)
		return;

	CWeapon* weapon = smart_cast<CWeapon*>(item);
	CHelmet* helmet = smart_cast<CHelmet*>(item);
	CCustomOutfit* outfit = smart_cast<CCustomOutfit*>(item);
	CCustomDetector* detector = smart_cast<CCustomDetector*>(item);
	CEatableItem* eatable = smart_cast<CEatableItem*>(item);
	CArtefact* artefact = smart_cast<CArtefact*>(item);
    u32 item_slot = item->BaseSlot();
	
    if (item_slot == BINOCULAR_SLOT)
    {
        m_BinocularSlotHighlight->Show(true);
        return;
    }
    if (item_slot == KNIFE_SLOT)
    {
        m_KnifeSlotHighlight->Show(true);
        return;
    }
	
	if (item_slot == INV_SLOT_2 || item_slot == INV_SLOT_3)
	{
		m_InvSlot2Highlight->Show(true);
		m_InvSlot3Highlight->Show(true);
		return;
	}
	if (item_slot == HELMET_SLOT)
	{
		m_HelmetSlotHighlight->Show(true);
		return;
	}
	if (item_slot == OUTFIT_SLOT)
	{
		m_OutfitSlotHighlight->Show(true);
		return;
	}
	if (item_slot == DETECTOR_SLOT)
	{
		m_DetectorSlotHighlight->Show(true);
		return;
	}
	CObject*	pObj = smart_cast<CObject*>		(item);
	shared_str	section_name = pObj->cNameSect();
	if(eatable)
	{
	bool CanSwitchToFastSlot = READ_IF_EXISTS(pSettings, r_bool, section_name, "can_switch_to_fast_slot", true);
	    if (!CanSwitchToFastSlot)
		    return;
		
		if(cell_item->OwnerList() && GetListType(cell_item->OwnerList())==iQuickSlot)
			return;

		for(u8 i=0; i<4; i++)
			m_QuickSlotsHighlight[i]->Show(true);
		return;
	}

	if(artefact)
	{
		if(cell_item->OwnerList() && GetListType(cell_item->OwnerList())==iActorBelt)
			return;

		Ivector2 cap = m_pInventoryBeltList->CellsCapacity();
        for (u8 i = 0; i < cap.y; i++)
        {
            for (u8 i = 0; i < cap.x; i++)
            {
			    m_ArtefactSlotsHighlight[i]->Show(true);
            }
        }
		return;
	}
}

#include "../WeaponKnife.h"
void CUIActorMenu::set_highlight_item( CUICellItem* cell_item )
{
	PIItem item = (PIItem)cell_item->m_pData;
	if ( !item )
	{
		return;
	}
	highlight_item_slot(cell_item);

	// не подсвечивать потроны для ножа
	if (smart_cast<CWeaponKnife*>(item))
	{
		return;
	}

	switch ( m_currMenuMode )
	{
	case mmUndefined:
	case mmInventory:
	case mmUpgrade:
		{
			highlight_armament( item, m_pInventoryBagList );
			break;
		}
	case mmTrade:
		{
			highlight_armament( item, m_pTradeActorBagList );
			highlight_armament( item, m_pTradeActorList );
			highlight_armament( item, m_pTradePartnerBagList );
			highlight_armament( item, m_pTradePartnerList );
			break;
		}
	case mmDeadBodySearch:
		{
			highlight_armament( item, m_pInventoryBagList );
			highlight_armament( item, m_pDeadBodyBagList );
			break;
		}
	}
	m_highlight_clear = false;
}

void CUIActorMenu::highlight_armament( PIItem item, CUIDragDropListEx* ddlist )
{
	ddlist->clear_select_armament();
	highlight_ammo_for_weapon( item, ddlist );
	highlight_weapons_for_ammo( item, ddlist );
	highlight_weapons_for_addon( item, ddlist );
}

void CUIActorMenu::highlight_ammo_for_weapon(PIItem weapon_item, CUIDragDropListEx* ddlist)
{
	VERIFY(weapon_item);
	VERIFY(ddlist);
	static xr_vector<shared_str>	ammo_types;
	ammo_types.clear();

	CWeapon* weapon = smart_cast<CWeapon*>(weapon_item);
	if (!weapon)
	{
		return;
	}
	ammo_types.assign(weapon->m_ammoTypes.begin(), weapon->m_ammoTypes.end());

	CWeaponMagazinedWGrenade* wg = smart_cast<CWeaponMagazinedWGrenade*>(weapon_item);
	if (wg)
	{
		if (wg->IsGrenadeLauncherAttached() && wg->m_ammoTypes2.size())
		{
			ammo_types.insert(ammo_types.end(), wg->m_ammoTypes2.begin(), wg->m_ammoTypes2.end());
		}
	}

	if (ammo_types.size() == 0)
	{
		return;
	}

	u32 const cnt = ddlist->ItemsCount();
	for (u32 i = 0; i < cnt; ++i)
	{
		CUICellItem* ci = ddlist->GetItemIdx(i);
		PIItem item = (PIItem)ci->m_pData;
		if (!item)
		{
			continue;
		}
		CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(item);
		if (!ammo)
		{
			highlight_addons_for_weapon(weapon_item, ci);
			continue; // for i
		}

		shared_str const& ammo_name = item->object().cNameSect();


		for (auto it : ammo_types)
		{
			if (ammo_name == it)
			{
				ci->m_select_armament = true;
				break; // itb
			}
		}
	}
}

void CUIActorMenu::highlight_weapons_for_ammo(PIItem ammo_item, CUIDragDropListEx* ddlist)
{
	VERIFY(ammo_item);
	VERIFY(ddlist);
	CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(ammo_item);
	if (!ammo)
	{
		return;
	}

	shared_str const& ammo_name = ammo_item->object().cNameSect();

	u32 const cnt = ddlist->ItemsCount();
	for (u32 i = 0; i < cnt; ++i)
	{
		CUICellItem* ci = ddlist->GetItemIdx(i);
		PIItem item = (PIItem)ci->m_pData;
		if (!item)
		{
			continue;
		}
		CWeapon* weapon = smart_cast<CWeapon*>(item);
		if (!weapon)
		{
			continue;
		}

		for (shared_str &it : weapon->m_ammoTypes)
		{
			if (ammo_name == it)
			{
				ci->m_select_armament = true;
				break; // itb 
			}
		}


		CWeaponMagazinedWGrenade* wg = smart_cast<CWeaponMagazinedWGrenade*>(item);
		if (!wg || !wg->IsGrenadeLauncherAttached() || !wg->m_ammoTypes2.size())
		{
			continue; // for i
		}

		for (shared_str &it : wg->m_ammoTypes2)
		{
			if (ammo_name == it)
			{
				ci->m_select_armament = true;
				break; // itb 
			}
		}

	}
}

bool CUIActorMenu::highlight_addons_for_weapon(PIItem weapon_item, CUICellItem* ci)
{
	PIItem item = (PIItem)ci->m_pData;
	if (!item)
	{
		return false;
	}

	CScope* pScope = smart_cast<CScope*>(item);
	if (pScope && weapon_item->CanAttach(pScope))
	{
		ci->m_select_armament = true;
		return true;
	}

	CSilencer* pSilencer = smart_cast<CSilencer*>(item);
	if (pSilencer && weapon_item->CanAttach(pSilencer))
	{
		ci->m_select_armament = true;
		return true;
	}

	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(item);
	if (pGrenadeLauncher && weapon_item->CanAttach(pGrenadeLauncher))
	{
		ci->m_select_armament = true;
		return true;
	}
	return false;
}

void CUIActorMenu::highlight_weapons_for_addon( PIItem addon_item, CUIDragDropListEx* ddlist )
{
	VERIFY( addon_item );
	VERIFY( ddlist );

	CScope*				pScope				= smart_cast<CScope*>			(addon_item);
	CSilencer*			pSilencer			= smart_cast<CSilencer*>		(addon_item);
	CGrenadeLauncher*	pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(addon_item);

	if ( !pScope && !pSilencer && !pGrenadeLauncher )
	{
		return;
	}
	
	u32 const cnt = ddlist->ItemsCount();
	for ( u32 i = 0; i < cnt; ++i )
	{
		CUICellItem* ci = ddlist->GetItemIdx(i);
		PIItem item = (PIItem)ci->m_pData;
		if ( !item )
		{
			continue;
		}
		CWeapon* weapon = smart_cast<CWeapon*>(item);
		if ( !weapon )
		{
			continue;
		}

		if ( pScope && weapon->CanAttach(pScope) )
		{
			ci->m_select_armament = true;
			continue;
		}
		if ( pSilencer && weapon->CanAttach(pSilencer) )
		{
			ci->m_select_armament = true;
			continue;
		}
		if ( pGrenadeLauncher && weapon->CanAttach(pGrenadeLauncher) )
		{
			ci->m_select_armament = true;
			continue;
		}

	}//for i
}

// -------------------------------------------------------------------

void CUIActorMenu::ClearAllLists()
{
	m_pInventoryBagList->ClearAll				(true);
	
	m_pInventoryBeltList->ClearAll				(true);
	m_pInventoryOutfitList->ClearAll			(true);
	m_pInventoryHelmetList->ClearAll			(true);
	m_pInventoryDetectorList->ClearAll			(true);
	m_pInventoryPistolList->ClearAll			(true);
	m_pInventoryAutomaticList->ClearAll			(true);

    if (g_extraFeatures.is(GAME_EXTRA_RUCK))
    {
        m_pInventoryRuckList->ClearAll(true);
    }

    m_pInventoryKnifeList->ClearAll             (true);
    m_pInventoryBinocularList->ClearAll         (true);
	
	m_pQuickSlot->ClearAll						(true);

	m_pTradeActorBagList->ClearAll				(true);
	m_pTradeActorList->ClearAll					(true);
	m_pTradePartnerBagList->ClearAll			(true);
	m_pTradePartnerList->ClearAll				(true);
	m_pDeadBodyBagList->ClearAll				(true);
}

void CUIActorMenu::CallMessageBoxYesNo( LPCSTR text )
{
	m_message_box_yes_no->SetText( text );
	m_message_box_yes_no->func_on_ok = CUIWndCallback::void_function( this, &CUIActorMenu::OnMesBoxYes );
	m_message_box_yes_no->func_on_no = CUIWndCallback::void_function( this, &CUIActorMenu::OnMesBoxNo );
	m_message_box_yes_no->ShowDialog(false);
}

void CUIActorMenu::CallMessageBoxOK( LPCSTR text )
{
	m_message_box_ok->SetText( text );
	m_message_box_ok->ShowDialog(false);
}

void CUIActorMenu::ResetMode()
{
	ClearAllLists				();
	m_pMouseCapturer			= NULL;
	m_UIPropertiesBox->Hide		();
	SetCurrentItem				(NULL);
    GameUI()->UIMainIngameWnd->ShowZoneMap(true);
}

bool CUIActorMenu::CanSetItemToList(PIItem item, CUIDragDropListEx* l, u16& ret_slot)
{
	u16 item_slot = item->BaseSlot();
	if( GetSlotList(item_slot)==l )
	{
		ret_slot	= item_slot;
		return		true;
	}

	if(item_slot==INV_SLOT_3 && l==m_pInventoryPistolList)
	{
		ret_slot	= INV_SLOT_2;
		return		true;
	}

	if(item_slot==INV_SLOT_2 && l==m_pInventoryAutomaticList)
	{
		ret_slot	= INV_SLOT_3;
		return		true;
	}

	return false;
}
void CUIActorMenu::UpdateConditionProgressBars()
{
	PIItem itm = m_pActorInvOwner->inventory().ItemFromSlot(INV_SLOT_2);
	if (itm)
	{
		m_WeaponSlot1_progress->SetProgressPos(iCeil(itm->GetCondition()*15.0f) / 15.0f);
	}
	else
		m_WeaponSlot1_progress->SetProgressPos(0);

	itm = m_pActorInvOwner->inventory().ItemFromSlot(INV_SLOT_3);
	if (itm)
		m_WeaponSlot2_progress->SetProgressPos(iCeil(itm->GetCondition()*15.0f) / 15.0f);
	else
		m_WeaponSlot2_progress->SetProgressPos(0);

	itm = m_pActorInvOwner->inventory().ItemFromSlot(OUTFIT_SLOT);
	if (itm)
		m_Outfit_progress->SetProgressPos(iCeil(itm->GetCondition()*15.0f) / 15.0f);
	else
		m_Outfit_progress->SetProgressPos(0);

	itm = m_pActorInvOwner->inventory().ItemFromSlot(HELMET_SLOT);
	if (itm)
		m_Helmet_progress->SetProgressPos(iCeil(itm->GetCondition()*15.0f) / 15.0f);
	else
		m_Helmet_progress->SetProgressPos(0);
	
	//Highlight 'equipped' items in actor bag
	CUIDragDropListEx* slot_list = m_pInventoryBagList;
	u32 const cnt = slot_list->ItemsCount();
	for (u32 i = 0; i < cnt; ++i)
	{
		CUICellItem* ci = slot_list->GetItemIdx(i);
		PIItem item = (PIItem)ci->m_pData;
		if (!item)
			continue;
		
		if (item->m_highlight_equipped && item->m_pInventory)
			ci->m_select_equipped = true;
		else
			ci->m_select_equipped = false;
	}	
}
#include "../ai_space.h"
#include "../../xrServerEntities/script_engine.h"

using namespace luabind;

void CUIActorMenu::TryRepairItem(CUIWindow* w, void* d)
{
	PIItem item = get_upgrade_item();
	if (!item)
	{
		return;
	}
	if (item->GetCondition() > 0.99f)
	{
		return;
	}
	LPCSTR item_name = item->m_section_id.c_str();
	LPCSTR partner = m_pPartnerInvOwner->CharacterInfo().Profile().c_str();

	luabind::functor<bool> funct;
	R_ASSERT2(
		ai().script_engine().functor("inventory_upgrades.can_repair_item", funct),
		make_string("Failed to get functor <inventory_upgrades.can_repair_item>, item = %s", item_name)
	);
	bool can_repair = funct(item_name, item->GetCondition(), partner);

	luabind::functor<LPCSTR> funct2;
	R_ASSERT2(
		ai().script_engine().functor("inventory_upgrades.question_repair_item", funct2),
		make_string("Failed to get functor <inventory_upgrades.question_repair_item>, item = %s", item_name)
	);
	LPCSTR question = funct2(item_name, item->GetCondition(), can_repair, partner);

	if (can_repair)
	{
		m_repair_mode = true;
		CallMessageBoxYesNo(question);
	}
	else
		CallMessageBoxOK(question);
}

void CUIActorMenu::RepairEffect_CurItem()
{
	PIItem item = CurrentIItem();
	if (!item)
	{
		return;
	}
	LPCSTR item_name = item->m_section_id.c_str();

	luabind::functor<void>	funct;
	R_ASSERT(ai().script_engine().functor("inventory_upgrades.effect_repair_item", funct));
	funct(item_name, item->GetCondition());

	item->SetCondition(1.0f);
	UpdateConditionProgressBars();
	SeparateUpgradeItem();
	CUICellItem* itm = CurrentItem();
	if (itm)
		itm->UpdateConditionProgressBar();

}

bool CUIActorMenu::CanUpgradeItem(PIItem item)
{
	VERIFY(item && m_pPartnerInvOwner);
	LPCSTR item_name = item->m_section_id.c_str();
	LPCSTR partner = m_pPartnerInvOwner->CharacterInfo().Profile().c_str();

	luabind::functor<bool> funct;
	R_ASSERT2(
		ai().script_engine().functor("inventory_upgrades.can_upgrade_item", funct),
		make_string("Failed to get functor <inventory_upgrades.can_upgrade_item>, item = %s, mechanic = %s", item_name, partner)
	);

	return funct(item_name, partner);
}

void CUIActorMenu::CurModeToScript()
{
	int mode = (int)m_currMenuMode;
	luabind::functor<void>	funct;
	R_ASSERT(ai().script_engine().functor("actor_menu.actor_menu_mode", funct));
	funct(mode);
}