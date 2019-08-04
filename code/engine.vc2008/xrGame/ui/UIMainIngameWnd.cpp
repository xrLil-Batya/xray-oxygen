#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "UIMessagesWindow.h"
#include "../UIZoneMap.h"

#include "../actor.h"
#include "../ActorCondition.h"
#include "../EntityCondition.h"
#include "../items/CustomOutfit.h"
#include "../items/Helmet.h"
#include "../PDA.h"
#include "../xrServerEntities/character_info.h"
#include "../inventory.h"
#include "../UIGame.h"
#include "../items/weaponmagazined.h"
#include "../missile.h"
#include "../items/Grenade.h"
#include "../xrServerEntities/xrServer_objects_ALife.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"
#include "../level.h"
#include "../seniority_hierarchy_holder.h"

#include "../xrEngine/date_time.h"
#include "../xrServerEntities/xrServer_Objects_ALife_Monsters.h"
#include "../../xrEngine/LightAnimLibrary.h"

#include "UIInventoryUtilities.h"
#include "../xrUICore/UIHelper.h"
#include "UIMotionIcon.h"

#include "../xrUICore/UIXmlInit.h"
#include "../xrUICore/UIPdaMsgListItem.h"
#include "UIPdaWnd.h"
#include "../alife_registry_wrappers.h"

#include "../xrEngine/string_table.h"

#ifdef DEBUG
#	include "../attachable_item.h"
#	include "../../xrEngine/xr_input.h"
#endif

#include "../xrUICore/UIScrollView.h"
#include "map_hint.h"
#include "../game_news.h"

#include "UIHudStatesWnd.h"
#include "UIActorMenu.h"

#include "../Include/xrRender/Kinematics.h"


using namespace InventoryUtilities;
const u32	g_clWhite					= 0xffffffff;

#define		DEFAULT_MAP_SCALE			1.f

#define		C_SIZE						0.025f
#define		NEAR_LIM					0.5f

#define		SHOW_INFO_SPEED				0.5f
#define		HIDE_INFO_SPEED				10.f
#define		C_ON_ENEMY					D3DCOLOR_XRGB(0xff,0,0)
#define		C_DEFAULT					D3DCOLOR_XRGB(0xff,0xff,0xff)

#define				MAININGAME_XML				"ui_HUD.xml"

CUIMainIngameWnd::CUIMainIngameWnd() : m_pPickUpItem(nullptr), UIArtefactIcon(nullptr)
{
	UIZoneMap					= xr_new<CUIZoneMap>();
}

#include "../xrUICore/UIProgressShape.h"
extern CUIProgressShape* g_MissileForceShape;

CUIMainIngameWnd::~CUIMainIngameWnd()
{
	DestroyFlashingIcons		();
	xr_delete					(UIZoneMap);
	HUD_SOUND_ITEM::DestroySound(m_contactSnd);
	xr_delete					(g_MissileForceShape);
	xr_delete					(UIWeaponJammedIcon);
	xr_delete					(UIInvincibleIcon);
	xr_delete					(UIArtefactIcon);
}

void CUIMainIngameWnd::Init()
{
	CUIXml						uiXml;
	uiXml.Load					(CONFIG_PATH, UI_PATH, MAININGAME_XML);
	
	CUIXmlInit					xml_init;
	xml_init.InitWindow			(uiXml,"main",0,this);

	Enable(false);
	//---------------------------------------------------------
	UIPickUpItemIcon			= UIHelper::CreateStatic		(uiXml, "pick_up_item", this);
	UIPickUpItemIcon->SetShader	(GetEquipmentIconsShader());

	m_iPickUpItemIconWidth		= UIPickUpItemIcon->GetWidth();
	m_iPickUpItemIconHeight		= UIPickUpItemIcon->GetHeight();
	m_iPickUpItemIconX			= UIPickUpItemIcon->GetWndRect().left;
	m_iPickUpItemIconY			= UIPickUpItemIcon->GetWndRect().top;
	//---------------------------------------------------------

	//индикаторы 
	UIZoneMap->Init				();

	// Подсказки, которые возникают при наведении прицела на объект
	UIStaticQuickHelp			= UIHelper::CreateTextWnd(uiXml, "quick_info", this);

	uiXml.SetLocalRoot			(uiXml.GetRoot());

	m_UIIcons					= xr_new<CUIScrollView>(); m_UIIcons->SetAutoDelete(true);
	xml_init.InitScrollView		(uiXml, "icons_scroll_view", 0, m_UIIcons);
	AttachChild					(m_UIIcons);
	
	m_ind_bleeding			= UIHelper::CreateStatic(uiXml, "indicator_bleeding", this);
	m_ind_radiation			= UIHelper::CreateStatic(uiXml, "indicator_radiation", this);
	m_ind_starvation		= UIHelper::CreateStatic(uiXml, "indicator_starvation", this);
    if (g_extraFeatures.is(GAME_EXTRA_THIRST))
    {
	    m_ind_thirst			= UIHelper::CreateStatic(uiXml, "indicator_thirst", this);
    }
	m_ind_weapon_broken		= UIHelper::CreateStatic(uiXml, "indicator_weapon_broken", this);
	m_ind_helmet_broken		= UIHelper::CreateStatic(uiXml, "indicator_helmet_broken", this);
	m_ind_outfit_broken		= UIHelper::CreateStatic(uiXml, "indicator_outfit_broken", this);
	m_ind_overweight		= UIHelper::CreateStatic(uiXml, "indicator_overweight", this);

	m_ind_boost_psy			= UIHelper::CreateStatic(uiXml, "indicator_booster_psy", this);
	m_ind_boost_radia		= UIHelper::CreateStatic(uiXml, "indicator_booster_radia", this);
	m_ind_boost_chem		= UIHelper::CreateStatic(uiXml, "indicator_booster_chem", this);
	m_ind_boost_wound		= UIHelper::CreateStatic(uiXml, "indicator_booster_wound", this);
	m_ind_boost_weight		= UIHelper::CreateStatic(uiXml, "indicator_booster_weight", this);
	m_ind_boost_health		= UIHelper::CreateStatic(uiXml, "indicator_booster_health", this);
	m_ind_boost_power		= UIHelper::CreateStatic(uiXml, "indicator_booster_power", this);
	m_ind_boost_rad			= UIHelper::CreateStatic(uiXml, "indicator_booster_rad", this);
	m_ind_boost_psy			->Show(false);
	m_ind_boost_radia		->Show(false);
	m_ind_boost_chem		->Show(false);
	m_ind_boost_wound		->Show(false);
	m_ind_boost_weight		->Show(false);
	m_ind_boost_health		->Show(false);
	m_ind_boost_power		->Show(false);
	m_ind_boost_rad			->Show(false);

	UIWeaponJammedIcon			= UIHelper::CreateStatic(uiXml, "weapon_jammed_static", NULL);
	UIWeaponJammedIcon->Show	(false);

	UIInvincibleIcon			= UIHelper::CreateStatic(uiXml, "invincible_static", NULL);
	UIInvincibleIcon->Show		(false);
	
    //#TODO: [Giperion] WTF IS THIS SHIT????????????

	shared_str warningStrings[8] = 
	{	
		"jammed",
		"radiation",
		"wounds",
		"starvation",
		"thirst",
		"fatigue",
		"invincible",
		"artefact"
	};

    shared_str classicWarningStrings[7] =
    {
        "jammed",
        "radiation",
        "wounds",
        "starvation",
        "fatigue",
        "invincible",
        "artefact"
    };

    shared_str* pUsedWarningStrings = g_extraFeatures.is(GAME_EXTRA_THIRST) ? &warningStrings[0] : &classicWarningStrings[0];

	// Загружаем пороговые значения для индикаторов
	EWarningIcons j = ewiWeaponJammed;
	while (j < ewiInvincible)
	{
		// Читаем данные порогов для каждого индикатора
		shared_str cfgRecord = pSettings->r_string("main_ingame_indicators_thresholds", *pUsedWarningStrings[static_cast<int>(j) - 1]);
		u32 count = _GetItemCount(*cfgRecord);

		char	singleThreshold[8];
		float	f = 0;
		for (u32 k = 0; k < count; ++k)
		{
			_GetItem(*cfgRecord, k, singleThreshold);
			sscanf(singleThreshold, "%f", &f);

			m_Thresholds[j].push_back(f);
		}

		j = static_cast<EWarningIcons>(j + 1);
	}


	// Flashing icons initialize
	uiXml.SetLocalRoot						(uiXml.NavigateToNode("flashing_icons"));
	InitFlashingIcons						(&uiXml);

	uiXml.SetLocalRoot						(uiXml.GetRoot());
	
	UIMotionIcon							= xr_new<CUIMotionIcon>(); UIMotionIcon->SetAutoDelete(true);
	UIZoneMap->MapFrame().AttachChild		(UIMotionIcon);
	UIMotionIcon->Init						(UIZoneMap->MapFrame().GetWndRect());

	UIStaticDiskIO							= UIHelper::CreateStatic(uiXml, "disk_io", this);

	m_ui_hud_states							= xr_new<CUIHudStatesWnd>();
	m_ui_hud_states->SetAutoDelete			(true);
	AttachChild								(m_ui_hud_states);
	m_ui_hud_states->InitFromXml			(uiXml, "hud_states");

	for(int i=0; i<4; i++)
	{
		m_quick_slots_icons.push_back	(xr_new<CUIStatic>());
		m_quick_slots_icons.back()	->SetAutoDelete(true);
		AttachChild				(m_quick_slots_icons.back());
		string32 path;
		xr_sprintf				(path, "quick_slot%d", i);
		CUIXmlInit::InitStatic	(uiXml, path, 0, m_quick_slots_icons.back());
		xr_sprintf				(path, "%s:counter", path);
		UIHelper::CreateStatic	(uiXml, path, m_quick_slots_icons.back());
	}
	m_QuickSlotText1				= UIHelper::CreateTextWnd(uiXml, "quick_slot0_text", this);
	m_QuickSlotText2				= UIHelper::CreateTextWnd(uiXml, "quick_slot1_text", this);
	m_QuickSlotText3				= UIHelper::CreateTextWnd(uiXml, "quick_slot2_text", this);
	m_QuickSlotText4				= UIHelper::CreateTextWnd(uiXml, "quick_slot3_text", this);

	HUD_SOUND_ITEM::LoadSound				("maingame_ui", "snd_new_contact", m_contactSnd, SOUND_TYPE_IDLE);
}

float UIStaticDiskIO_start_time = 0.0f;
void CUIMainIngameWnd::Draw()
{
	CActor* pActor		= smart_cast<CActor*>(Level().CurrentViewEntity());

	// show IO icon
	bool IOActive	= (FS.dwOpenCounter>0);
	if	(IOActive)	UIStaticDiskIO_start_time = Device.fTimeGlobal;

	if ((UIStaticDiskIO_start_time+1.0f) < Device.fTimeGlobal)	UIStaticDiskIO->Show(false); 
	else {
		u32		alpha			= clampr(iFloor(255.f*(1.f-(Device.fTimeGlobal-UIStaticDiskIO_start_time)/1.f)),0,255);
		UIStaticDiskIO->Show		( true  ); 
		UIStaticDiskIO->SetTextureColor(color_rgba(255,255,255,alpha));
	}
	FS.dwOpenCounter = 0;

	if(!pActor || !pActor->g_Alive()) 
		return;

	UIMotionIcon->SetNoise((s16)(0xffff&iFloor(pActor->m_snd_noise*100)));

	UIMotionIcon->Draw();
	UIZoneMap->Render();

	bool tmp = UIMotionIcon->IsShown();
	UIMotionIcon->Show(false);
	CUIWindow::Draw();
	UIMotionIcon->Show(tmp);

	RenderQuickInfos();		
}

void CUIMainIngameWnd::Update()
{
	CUIWindow::Update();
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	if (!pActor)
		return;

	UIZoneMap->Update();
	UpdatePickUpItem();

	if (Device.dwFrame % 10)
		return;

	bool b_God = GodMode();
	if (b_God)
		SetWarningIconColor(ewiInvincible, 0xffffffff);
	else
		SetWarningIconColor(ewiInvincible, 0x00ffffff);

	UpdateMainIndicators();
	return;

}

void CUIMainIngameWnd::RenderQuickInfos()
{
	CActor* pActor		= smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!pActor)
		return;

	static CGameObject *pObject			= NULL;
	LPCSTR actor_action					= pActor->GetDefaultActionForObject();
	UIStaticQuickHelp->Show				(NULL!=actor_action);

	// подсказка для костра
	static CZoneCampfire* pZone = nullptr;
	if (pZone != pActor->CapmfireWeLookingAt())
	{
		UIStaticQuickHelp->SetTextST(actor_action);
		UIStaticQuickHelp->ResetColorAnimation();
		pZone = pActor->CapmfireWeLookingAt();
	}

	if(NULL!=actor_action)
	{
		if(stricmp(actor_action,UIStaticQuickHelp->GetText()))
			UIStaticQuickHelp->SetTextST				(actor_action);
	}

	if(pObject!=pActor->ObjectWeLookingAt())
	{
		UIStaticQuickHelp->SetTextST				(actor_action?actor_action:" ");
		UIStaticQuickHelp->ResetColorAnimation	();
		pObject	= pActor->ObjectWeLookingAt	();
	}
}

void CUIMainIngameWnd::ReceiveNews(GAME_NEWS_DATA* news)
{
	VERIFY(news->texture_name.size());

	GameUI()->m_pMessagesWnd->AddIconedPdaMessage(news);
	GameUI()->UpdatePda();
}

void CUIMainIngameWnd::SetWarningIconColorUI(CUIStatic* s, const u32 cl)
{
	int bOn = ( cl >> 24 );
	bool bIsShown = s->IsShown();

	if ( bOn )
	{
		s->SetTextureColor( cl );
	}

	if ( bOn && !bIsShown )
	{
		m_UIIcons->AddWindow	(s, false);
		s->Show					(true);
	}

	if ( !bOn && bIsShown )
	{
		m_UIIcons->RemoveWindow	(s);
		s->Show					(false);
	}
}

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	bool bMagicFlag = true;

	// Задаем цвет требуемой иконки
	switch(icon)
	{
	case ewiAll:
		bMagicFlag = false;
	case ewiWeaponJammed:
		SetWarningIconColorUI	(UIWeaponJammedIcon, cl);
		if (bMagicFlag) break;
	case ewiInvincible:
		SetWarningIconColorUI	(UIInvincibleIcon, cl);
		if (bMagicFlag) break;
		break;
	case ewiArtefact:
		SetWarningIconColorUI	(UIArtefactIcon, cl);
		break;

	default:
		R_ASSERT(!"Unknown warning icon type");
	}
}

void CUIMainIngameWnd::TurnOffWarningIcon(EWarningIcons icon)
{
	SetWarningIconColor(icon, 0x00ffffff);
}

void CUIMainIngameWnd::SetFlashIconState_(EFlashingIcons type, bool enable)
{
	// Включаем анимацию требуемой иконки
    auto icon = m_FlashingIcons.find(type);
	R_ASSERT2(icon != m_FlashingIcons.end(), "Flashing icon with this type not existed");
	icon->second->Show(enable);
}

void CUIMainIngameWnd::InitFlashingIcons(CUIXml* node)
{
	const char * const flashingIconNodeName = "flashing_icon";
	int staticsCount = node->GetNodesNum("", 0, flashingIconNodeName);

	CUIXmlInit xml_init;
	CUIStatic *pIcon = NULL;
	// Пробегаемся по всем нодам и инициализируем из них статики
	for (int i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>();
		xml_init.InitStatic(*node, flashingIconNodeName, i, pIcon);
		shared_str iconType = node->ReadAttrib(flashingIconNodeName, i, "type", "none");

		// Теперь запоминаем иконку и ее тип
		EFlashingIcons type = efiPdaTask;

		if		(iconType == "pda")		type = efiPdaTask;
		else if (iconType == "mail")	type = efiMail;
		else	R_ASSERT(!"Unknown type of mainingame flashing icon");

		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end(), "Flashing icon with this type already exists");

		CUIStatic* &val	= m_FlashingIcons[type];
		val			= pIcon;

		AttachChild(pIcon);
		pIcon->Show(false);
	}
}

void CUIMainIngameWnd::DestroyFlashingIcons()
{
	for (auto it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		DetachChild(it->second);
		xr_delete(it->second);
	}

	m_FlashingIcons.clear();
}

void CUIMainIngameWnd::UpdateFlashingIcons()
{
	for (auto it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		it->second->Update();
	}
}

void CUIMainIngameWnd::AnimateContacts(bool b_snd)
{
	UIZoneMap->Counter_ResetClrAnimation();

	if(b_snd && !psActorFlags.test(AF_HARDCORE))
		HUD_SOUND_ITEM::PlaySound	(m_contactSnd, Fvector().set(0,0,0), 0, true );
	else
		HUD_SOUND_ITEM::StopSound(m_contactSnd);

}


void CUIMainIngameWnd::SetPickUpItem	(CInventoryItem* PickUpItem)
{
	m_pPickUpItem = PickUpItem;
};

void CUIMainIngameWnd::UpdatePickUpItem	()
{
	if (!m_pPickUpItem || !Level().CurrentViewEntity() || !smart_cast<CActor*>(Level().CurrentViewEntity())) 
	{
		UIPickUpItemIcon->Show(false);
		return;
	}

	shared_str sect_name = m_pPickUpItem->object().cNameSect();

	//properties used by inventory menu
	int m_iGridWidth	= pSettings->r_u32(sect_name, "inv_grid_width");
	int m_iGridHeight	= pSettings->r_u32(sect_name, "inv_grid_height");

	int m_iXPos			= pSettings->r_u32(sect_name, "inv_grid_x");
	int m_iYPos			= pSettings->r_u32(sect_name, "inv_grid_y");

	float scale_x = m_iPickUpItemIconWidth/
		float(m_iGridWidth*INV_GRID_WIDTH);

	float scale_y = m_iPickUpItemIconHeight/
		float(m_iGridHeight*INV_GRID_HEIGHT);

	scale_x = (scale_x>1) ? 1.0f : scale_x;
	scale_y = (scale_y>1) ? 1.0f : scale_y;

	float scale = scale_x<scale_y?scale_x:scale_y;

	Frect texture_rect;
	texture_rect.lt.set(m_iXPos*INV_GRID_WIDTH, m_iYPos*INV_GRID_HEIGHT);
	texture_rect.rb.set(m_iGridWidth*INV_GRID_WIDTH, m_iGridHeight*INV_GRID_HEIGHT);
	texture_rect.rb.add(texture_rect.lt);
	UIPickUpItemIcon->GetStaticItem()->SetTextureRect(texture_rect);
	UIPickUpItemIcon->SetStretchTexture(true);

	UIPickUpItemIcon->SetWidth(m_iGridWidth*INV_GRID_WIDTH*scale*UI().get_current_kx());
	UIPickUpItemIcon->SetHeight(m_iGridHeight*INV_GRID_HEIGHT*scale);

	if (psActorFlags.test(AF_COLORED_FEEL))
	{
		Fmatrix			res;
		res.mul(Device.mFullTransform, m_pPickUpItem->object().XFORM());
		Fvector4		v_res;
		Fvector			result = m_pPickUpItem->object().Position();
		Device.mFullTransform.transform(v_res, result);
		if (v_res.z < 0 || v_res.w < 0)	return;
		if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

		float x = (1.f + v_res.x) / 2.f * (Device.dwWidth);
		float y = (1.f - v_res.y) / 2.f * (Device.dwHeight);
		CPhysicsShellHolder& IconObj = m_pPickUpItem->object();
		UIPickUpItemIcon->SetWndPos(Fvector2().set(x / 1.7f + m_pPickUpItem->object().Radius(),
				y + (m_iPickUpItemIconHeight - UIPickUpItemIcon->GetHeight()) / 1.7f + IconObj.Radius() + Actor()->Position().distance_to(IconObj.Position()) / -1.5));
	}
	else
	{
		UIPickUpItemIcon->SetWndPos(Fvector2().set(m_iPickUpItemIconX + (m_iPickUpItemIconWidth - UIPickUpItemIcon->GetWidth()) / 2.0f,
			m_iPickUpItemIconY + (m_iPickUpItemIconHeight - UIPickUpItemIcon->GetHeight()) / 2.0f));
	}

	UIPickUpItemIcon->SetTextureColor(color_rgba(255,255,255,192));
	UIPickUpItemIcon->Show(true);
};

void CUIMainIngameWnd::OnConnected()
{
	// Init UIMap
	UIZoneMap->SetupCurrentMap();

	if (m_ui_hud_states)
	{
		m_ui_hud_states->on_connected();
	}
}

void CUIMainIngameWnd::OnSectorChanged(int sector)
{
	UIZoneMap->OnSectorChanged(sector);
}

void CUIMainIngameWnd::reset_ui()
{
	m_pPickUpItem					= NULL;
	UIMotionIcon->ResetVisibility	();
	if ( m_ui_hud_states )
	{
		m_ui_hud_states->reset_ui();
	}
}

void CUIMainIngameWnd::ShowZoneMap( bool status ) 
{ 
	UIZoneMap->visible = status; 
}

void CUIMainIngameWnd::DrawZoneMap() 
{ 
	UIZoneMap->Render(); 
}

void CUIMainIngameWnd::UpdateZoneMap() 
{ 
	UIZoneMap->Update(); 
}

CUIZoneMap* CUIMainIngameWnd::GetZoneMap()
{
    R_ASSERT2(UIZoneMap != nullptr, "Zone map is not created yet");
    return UIZoneMap;
}

void CUIMainIngameWnd::UpdateMainIndicators()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if(!pActor)
		return;

	UpdateQuickSlots();
	GameUI()->PdaMenu().UpdateRankingWnd(); 

	u8 flags = LA_CYCLIC | LA_ONLYALPHA | LA_TEXTURECOLOR;
	auto MakeIcon = [this](bool bShow, CUIStatic * pStatic, float Koef, float DefKoef1, float DefKoef2, const shared_str Sections[3])
	{
		pStatic->Show(bShow);
		if (bShow)
		{
			if (Koef < DefKoef1)
				pStatic->InitTexture(Sections[0].c_str());
			else if (Koef < DefKoef2)
				pStatic->InitTexture(Sections[1].c_str());
			else
				pStatic->InitTexture(Sections[2].c_str());
		}
	};
	auto MakeAnmIcon = [this, &flags](bool bShow, CUIStatic * pStatic, float Koef, float DefKoef1, float DefKoef2, const shared_str Sections[6])
	{
		pStatic->Show(bShow);
		if (bShow)
		{
			if (Koef < DefKoef1)
			{
				pStatic->InitTexture(Sections[0].c_str());
				pStatic->SetColorAnimation(Sections[3].c_str(), flags);
			}
			else if (Koef < DefKoef2)
			{
				pStatic->InitTexture(Sections[1].c_str());
				pStatic->SetColorAnimation(Sections[4].c_str(), flags);
			}
			else
			{
				pStatic->InitTexture(Sections[2].c_str());
				pStatic->SetColorAnimation(Sections[5].c_str(), flags);
			}
		}
		else pStatic->ResetColorAnimation();
	};

	// Bleeding icon
	{
		float bleeding = pActor->conditions().BleedingSpeed();
		static const shared_str Sects[6] =
		{
			"ui_inGame2_circle_bloodloose_green", "ui_inGame2_circle_bloodloose_yellow","ui_inGame2_circle_bloodloose_red",
			"ui_slow_blinking_alpha", "ui_medium_blinking_alpha", "ui_fast_blinking_alpha"
		};
		MakeAnmIcon(!fis_zero(bleeding, EPS), m_ind_bleeding, bleeding, 0.35f, 0.7f, Sects);
	}

	// Radiation icon
	{
		float radiation = pActor->conditions().GetRadiation();
		static const shared_str Sects[6] =
		{
			"ui_inGame2_circle_radiation_green", "ui_inGame2_circle_radiation_yellow", "ui_inGame2_circle_radiation_red",
			"ui_medium_blinking_alpha", "ui_medium_blinking_alpha", "ui_fast_blinking_alpha"
		};
		MakeAnmIcon(!fis_zero(radiation, EPS), m_ind_radiation, radiation, 0.35f, 0.7f, Sects);
	}

	// Satiety icon
	{
		const float satiety = pActor->conditions().GetSatiety();
		const float satiety_critical = pActor->conditions().SatietyCritical();
		const float satiety_koef = (satiety - satiety_critical) / (satiety >= satiety_critical ? 1 - satiety_critical : satiety_critical);

		static const shared_str Sects[3] =
		{
			"ui_inGame2_circle_hunger_green", "ui_inGame2_circle_hunger_yellow","ui_inGame2_circle_hunger_red"
		};
		MakeIcon(satiety_koef <= 0.5, m_ind_starvation, satiety_koef, 0.f, -5.f, Sects);
	}

	// Thirst icon
    if (g_extraFeatures.is(GAME_EXTRA_THIRST))
    {
        float thirst = pActor->conditions().GetThirst();
        float thirst_critical = pActor->conditions().ThirstCritical();
        float thirst_koef = (thirst - thirst_critical) / (thirst >= thirst_critical ? 1 - thirst_critical : thirst_critical);
		static const shared_str Sects[3] =
		{
			"ui_inGame2_circle_thirst_green", "ui_inGame2_circle_thirst_yellow","ui_inGame2_circle_thirst_red"
		};
		MakeIcon(thirst_koef <= 0.5, m_ind_thirst, thirst_koef, 0.f, -5.f, Sects);
    }

	// Armor broken icon
	m_ind_outfit_broken->Show(false);
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(pActor->inventory().ItemFromSlot(OUTFIT_SLOT));
	if (pOutfit)
	{
		float condition = pOutfit->GetCondition();
		static const shared_str Sects[3] =
		{
			"ui_inGame2_circle_Armorbroken_green", "ui_inGame2_circle_Armorbroken_yellow", "ui_inGame2_circle_Armorbroken_red"
		};
		MakeIcon(condition < 0.75f, m_ind_outfit_broken, condition, 0.5f, 0.25f, Sects);
	} m_ind_outfit_broken->Show(false);

	// Helmet broken icon
	CHelmet* pHelmet = smart_cast<CHelmet*>(pActor->inventory().ItemFromSlot(HELMET_SLOT));
	if(pHelmet)
	{
		float condition = pHelmet->GetCondition();
		static const shared_str Sects[3] =
		{
			"ui_inGame2_circle_Helmetbroken_green", "ui_inGame2_circle_Helmetbroken_yellow", "ui_inGame2_circle_Helmetbroken_red"
		};
		MakeIcon(condition < 0.75f, m_ind_helmet_broken, condition, 0.5f, 0.25f, Sects);
	}
	else m_ind_helmet_broken->Show(false);

	// Weapon broken icon
	u16 slot = pActor->inventory().GetActiveSlot();
	if(slot==INV_SLOT_2 || slot==INV_SLOT_3)
	{
		CWeapon* weapon = smart_cast<CWeapon*>(pActor->inventory().ItemFromSlot(slot));
		if(weapon)
		{
			float condition = weapon->GetCondition();
			float start_misf_cond = weapon->GetMisfireStartCondition();
			float end_misf_cond = weapon->GetMisfireEndCondition();
			static const shared_str Sects[3] =
			{
				"ui_inGame2_circle_Gunbroken_green", "ui_inGame2_circle_Gunbroken_yellow", "ui_inGame2_circle_Gunbroken_red"
			};
			MakeIcon(condition < start_misf_cond, m_ind_weapon_broken, condition, (start_misf_cond + end_misf_cond) / 2, end_misf_cond, Sects);
		}
		else m_ind_weapon_broken->Show(false);
	}
	else m_ind_weapon_broken->Show(false);

	// Overweight icon
	float cur_weight = pActor->inventory().TotalWeight();
	float max_weight = pActor->MaxWalkWeight();
	m_ind_overweight->Show(false);
	if(cur_weight>=max_weight-10.0f)
	{
		m_ind_overweight->Show(true);
		if(cur_weight>max_weight)
			m_ind_overweight->InitTexture("ui_inGame2_circle_Overweight_red");
		else
			m_ind_overweight->InitTexture("ui_inGame2_circle_Overweight_yellow");
	}
}

void CUIMainIngameWnd::UpdateQuickSlots()
{
	string32 tmp;
	LPCSTR str = CStringTable().translate("quick_use_str_1").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if(tmp[2]==',')
		tmp[1] = '\0';
	m_QuickSlotText1->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_2").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if(tmp[2]==',')
		tmp[1] = '\0';
	m_QuickSlotText2->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_3").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if(tmp[2]==',')
		tmp[1] = '\0';
	m_QuickSlotText3->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_4").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if(tmp[2]==',')
		tmp[1] = '\0';
	m_QuickSlotText4->SetTextST(tmp);


	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if(!pActor)
		return;

	for(u8 i=0;i<4;i++)
	{
		CUIStatic* wnd = smart_cast<CUIStatic* >(m_quick_slots_icons[i]->FindChild("counter"));
		if(wnd)
		{
			shared_str item_name = g_quick_use_slots[i];
			if(item_name.size())
			{
				u32 count = pActor->inventory().dwfGetSameItemCount(item_name.c_str(), true);
				string32 str;
				xr_sprintf(str, "x%d", count);
				wnd->TextItemControl()->SetText(str);
				wnd->Show(true);

				CUIStatic* main_slot = m_quick_slots_icons[i];
				main_slot->SetShader(InventoryUtilities::GetEquipmentIconsShader());
				Frect texture_rect;
				texture_rect.x1	= pSettings->r_float(item_name, "inv_grid_x")		*INV_GRID_WIDTH;
				texture_rect.y1	= pSettings->r_float(item_name, "inv_grid_y")		*INV_GRID_HEIGHT;
				texture_rect.x2	= pSettings->r_float(item_name, "inv_grid_width")	*INV_GRID_WIDTH;
				texture_rect.y2	= pSettings->r_float(item_name, "inv_grid_height")*INV_GRID_HEIGHT;
				texture_rect.rb.add(texture_rect.lt);
				main_slot->SetTextureRect(texture_rect);
				main_slot->TextureOn();
				main_slot->SetStretchTexture(true);
				if(!count)
				{
					wnd->SetTextureColor(color_rgba(255,255,255,0));
					wnd->TextItemControl()->SetTextColor(color_rgba(255,255,255,0));
					m_quick_slots_icons[i]->SetTextureColor(color_rgba(255,255,255,100));
				}
				else
				{
					wnd->SetTextureColor(color_rgba(255,255,255,255));
					wnd->TextItemControl()->SetTextColor(color_rgba(255,255,255,255));
					m_quick_slots_icons[i]->SetTextureColor(color_rgba(255,255,255,255));
				}
			}
			else
			{
				wnd->Show(false);
				m_quick_slots_icons[i]->SetTextureColor(color_rgba(255,255,255,0));
			}
		}
	}
}

void CUIMainIngameWnd::DrawMainIndicatorsForInventory()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if(!pActor)
		return;

	UpdateQuickSlots();
	UpdateBoosterIndicators(pActor->conditions().GetCurBoosterInfluences());

	for(int i=0;i<4;i++)
		m_quick_slots_icons[i]->Draw();

	m_QuickSlotText1->Draw();
	m_QuickSlotText2->Draw();
	m_QuickSlotText3->Draw();
	m_QuickSlotText4->Draw();

	if(m_ind_boost_psy->IsShown())
	{
		m_ind_boost_psy->Update();
		m_ind_boost_psy->Draw();
	}

	if(m_ind_boost_radia->IsShown())
	{
		m_ind_boost_radia->Update();
		m_ind_boost_radia->Draw();
	}

	if(m_ind_boost_chem->IsShown())
	{
		m_ind_boost_chem->Update();
		m_ind_boost_chem->Draw();
	}

	if(m_ind_boost_wound->IsShown())
	{
		m_ind_boost_wound->Update();
		m_ind_boost_wound->Draw();
	}

	if(m_ind_boost_weight->IsShown())
	{
		m_ind_boost_weight->Update();
		m_ind_boost_weight->Draw();
	}

	if(m_ind_boost_health->IsShown())
	{
		m_ind_boost_health->Update();
		m_ind_boost_health->Draw();
	}

	if(m_ind_boost_power->IsShown())
	{
		m_ind_boost_power->Update();
		m_ind_boost_power->Draw();
	}

	if(m_ind_boost_rad->IsShown())
	{
		m_ind_boost_rad->Update();
		m_ind_boost_rad->Draw();
	}

	m_ui_hud_states->DrawZoneIndicators();
}

void CUIMainIngameWnd::UpdateBoosterIndicators(const xr_map<EBoostParams, SBooster>& influences)
{
	m_ind_boost_psy->Show(false);
	m_ind_boost_radia->Show(false);
	m_ind_boost_chem->Show(false);
	m_ind_boost_wound->Show(false);
	m_ind_boost_weight->Show(false);
	m_ind_boost_health->Show(false);
	m_ind_boost_power->Show(false);
	m_ind_boost_rad->Show(false);

	LPCSTR str_flag	= "ui_slow_blinking_alpha";
	u8 flags = 0;
	flags |= LA_CYCLIC;
	flags |= LA_ONLYALPHA;
	flags |= LA_TEXTURECOLOR;

	xr_map<EBoostParams, SBooster>::const_iterator b = influences.begin(), e = influences.end();
	for(; b != e; ++b)
	{
		switch(b->second.m_type)
		{
			case eBoostHpRestore: 
				{
					m_ind_boost_health->Show(true);
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_health->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_health->ResetColorAnimation();
				}
				break;
			case eBoostPowerRestore: 
				{
					m_ind_boost_power->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_power->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_power->ResetColorAnimation();
				}
				break;
			case eBoostRadiationRestore: 
				{
					m_ind_boost_rad->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_rad->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_rad->ResetColorAnimation();
				}
				break;
			case eBoostBleedingRestore: 
				{
					m_ind_boost_wound->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_wound->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_wound->ResetColorAnimation();
				}
				break;
			case eBoostMaxWeight: 
				{
					m_ind_boost_weight->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_weight->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_weight->ResetColorAnimation();
				}
				break;
			case eBoostRadiationImmunity: 
			case eBoostRadiationProtection: 
				{
					m_ind_boost_radia->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_radia->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_radia->ResetColorAnimation();
				}
				break;
			case eBoostTelepaticImmunity: 
			case eBoostTelepaticProtection: 
				{
					m_ind_boost_psy->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_psy->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_psy->ResetColorAnimation();
				}
				break;
			case eBoostChemicalBurnImmunity: 
			case eBoostChemicalBurnProtection: 
				{
					m_ind_boost_chem->Show(true); 
					if(b->second.fBoostTime<=3.0f)
						m_ind_boost_chem->SetColorAnimation(str_flag, flags);
					else
						m_ind_boost_chem->ResetColorAnimation();
				}
				break;
		}
	}
}