////////////////////////////////////////////////////////////////////////////
//	Module 		: UIInventoryUpgradeWnd.cpp
//	Created 	: 06.10.2007
//  Modified 	: 13.03.2009
//	Author		: Evgeniy Sokolov, Prishchepa Sergey
//	Description : inventory upgrade UI window class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_broker.h"
#include "UIInventoryUpgradeWnd.h"

#include "../xrUICore/xrUIXmlParser.h"
#include "../xrUICore/UIXmlInit.h"
#include "../xrEngine/string_table.h"

#include "../actor.h"
#include "../../xrServerEntities/script_process.h"
#include "../inventory.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "inventory_upgrade_manager.h"
#include "inventory_upgrade.h"
#include "inventory_upgrade_property.h"

#include "UIInventoryUtilities.h"
#include "UIActorMenu.h"
#include "UIItemInfo.h"
#include "../xrUICore/UIFrameLineWnd.h"
#include "../xrUICore/UI3tButton.h"
#include "../xrUICore/UIHelper.h"
#include "../../xrUICore/ui_defs.h"
#include "../items/Weapon.h"
#include "../items/WeaponRPG7.h"
#include "../items/CustomOutfit.h"
#include "../items/Helmet.h"

// -----
const char* const g_inventory_upgrade_xml = "inventory_upgrade.xml";

CUIInventoryUpgradeWnd::Scheme::Scheme()
{
}

CUIInventoryUpgradeWnd::Scheme::~Scheme()
{
	delete_data( cells );
}

// =============================================================================================

CUIInventoryUpgradeWnd::CUIInventoryUpgradeWnd()
{
	m_inv_item       = NULL;
	m_cur_upgrade_id = NULL;
	m_current_scheme = NULL;
	m_btn_repair     = NULL;
}

CUIInventoryUpgradeWnd::~CUIInventoryUpgradeWnd()
{
	delete_data( m_schemes );
}

void CUIInventoryUpgradeWnd::Init()
{
	CUIXml uiXml;
	uiXml.Load( CONFIG_PATH, UI_PATH, g_inventory_upgrade_xml );
	
	CUIXmlInit xml_init;
	xml_init.InitWindow( uiXml, "main", 0, this );

	m_item = xr_new<CUIStatic>();
	m_item->SetAutoDelete( true );
	AttachChild( m_item );
	xml_init.InitStatic( uiXml, "item_static", 0, m_item );
	
	m_back = xr_new<CUIWindow>();
	m_back->SetAutoDelete( true );
	xml_init.InitWindow( uiXml, "back", 0, m_back );
	AttachChild(m_back);

	m_scheme_wnd = xr_new<CUIWindow>();
	m_scheme_wnd->SetAutoDelete( true );
	AttachChild( m_scheme_wnd );
	xml_init.InitWindow( uiXml, "scheme", 0, m_scheme_wnd );

	m_btn_repair = UIHelper::Create3tButton( uiXml, "repair_button", this );

	LoadCellsBacks( uiXml );
	LoadSchemes( uiXml );
}

void CUIInventoryUpgradeWnd::InitInventory( CInventoryItem* item, bool can_upgrade )
{
	m_inv_item = item;
	bool is_shader = false;

	// Загружаем картинку

	if (m_item)
	{
		if (smart_cast<CWeapon*>(item))
		{
			is_shader = true;
			m_item->SetShader(InventoryUtilities::GetWeaponUpgradeIconsShader());
		}
		else if (smart_cast<CCustomOutfit*>(item) || smart_cast<CHelmet*>(item))
		{
			is_shader = true;
			m_item->SetShader(InventoryUtilities::GetOutfitUpgradeIconsShader());
		}

		if (is_shader)
		{

			Irect item_upgrade_grid_rect = item->GetUpgrIconRect();
			Frect texture_rect;

			texture_rect.lt.set(item_upgrade_grid_rect.x1, item_upgrade_grid_rect.y1);
			texture_rect.rb.set(item_upgrade_grid_rect.x2, item_upgrade_grid_rect.y2);
			texture_rect.rb.add(texture_rect.lt);

			m_item->GetUIStaticItem().SetTextureRect(texture_rect);
			m_item->TextureOn();
			m_item->SetStretchTexture(true);

			Fvector2 v_r = Fvector2().set(item_upgrade_grid_rect.x2, item_upgrade_grid_rect.y2);
			if (UI().is_widescreen())
				v_r.x *= 0.8f;

			m_item->GetUIStaticItem().SetSize(v_r);
			m_item->SetWidth(v_r.x);
			m_item->SetHeight(v_r.y);
			m_item->Show(true);
		}
	}
	else m_item->Show(false);

	m_scheme_wnd->DetachAll();
	m_scheme_wnd->Show( false );
	m_back->DetachAll();
	m_back->Show(false);
	m_btn_repair->Enable( false );
	
	if ( ai().get_alife() && m_inv_item )
	{
		if ( install_item( *m_inv_item, can_upgrade ) )
		{
			UpdateAllUpgrades();
		}
	}
}

// ------------------------------------------------------------------------------------------

void CUIInventoryUpgradeWnd::Show( bool status )
{ 
	inherited::Show( status );
	UpdateAllUpgrades();
}

void CUIInventoryUpgradeWnd::Update()
{
	inherited::Update();
}

void CUIInventoryUpgradeWnd::Reset()
{
	SCHEMES::iterator ibw = m_schemes.begin();
	SCHEMES::iterator iew = m_schemes.end();
	for ( ; ibw != iew; ++ibw )
	{
		UI_Upgrades_type::iterator ib = (*ibw)->cells.begin();
		UI_Upgrades_type::iterator ie = (*ibw)->cells.end();
		for ( ; ib != ie; ++ib )
		{
			(*ib)->Reset();
			(*ib)->m_point->Reset();
		}
	}
	inherited::Reset();
	inherited::ResetAll();
}

void CUIInventoryUpgradeWnd::UpdateAllUpgrades()
{
	if ( !m_current_scheme || !m_inv_item )
	{
		return;
	}
	
	UI_Upgrades_type::iterator ib = m_current_scheme->cells.begin();
	UI_Upgrades_type::iterator ie = m_current_scheme->cells.end();
	for ( ; ib != ie; ++ib )
	{
		(*ib)->update_item( m_inv_item );
	}
}

void CUIInventoryUpgradeWnd::SetCurScheme( const shared_str& id )
{
	SCHEMES::iterator ib = m_schemes.begin();
	SCHEMES::iterator ie = m_schemes.end();
	for ( ; ib != ie; ++ib )
	{
		if ( (*ib)->name._get() == id._get() )
		{
			m_current_scheme = (*ib);
			return;
		}
	}
	VERIFY_FORMAT(0, "Scheme <%s> does not loaded !", id.c_str());
}

bool CUIInventoryUpgradeWnd::install_item( CInventoryItem& inv_item, bool can_upgrade )
{
	m_scheme_wnd->DetachAll();
	m_back->DetachAll();
	m_btn_repair->Enable( (inv_item.GetCondition() < 0.99f) );

	if ( !can_upgrade )
	{
#ifdef DEBUG
		Msg( "Inventory item <%s> cannot upgrade - Mechanic say.", inv_item.m_section_id.c_str() );
#endif // DEBUG
		m_current_scheme = NULL;
		return false;
	}

	LPCSTR scheme_name = get_manager().get_item_scheme( inv_item );
	if ( !scheme_name )
	{
#ifdef DEBUG
		Msg( "Inventory item <%s> does not contain upgrade scheme.", inv_item.m_section_id.c_str() );
#endif // DEBUG
		m_current_scheme = NULL;
		return false;
	}

	SetCurScheme( scheme_name );
	
	UI_Upgrades_type::iterator ib = m_current_scheme->cells.begin();
	UI_Upgrades_type::iterator ie = m_current_scheme->cells.end();
	for ( ; ib != ie; ++ib )
	{
		UIUpgrade* ui_item = (*ib);
		m_scheme_wnd->AttachChild( ui_item );
		m_back->AttachChild( ui_item->m_point );
		
		LPCSTR upgrade_name = get_manager().get_upgrade_by_index( inv_item, ui_item->get_scheme_index() );
		ui_item->init_upgrade( upgrade_name, inv_item );
		
		Upgrade_type* upgrade_p = get_manager().get_upgrade( upgrade_name );
		VERIFY( upgrade_p );
		for(u8 i = 0; i < inventory::upgrade::max_properties_count; i++)
		{
			shared_str prop_name = upgrade_p->get_property_name(i);
			if(prop_name.size())
			{
				Property_type* prop_p = get_manager().get_property( prop_name );
				VERIFY( prop_p );
			}
		}
		
		ui_item->set_texture( UIUpgrade::LAYER_ITEM,   upgrade_p->icon_name() );
		ui_item->set_texture( UIUpgrade::LAYER_POINT,  m_point_textures[UIUpgrade::STATE_ENABLED].c_str() ); //default
		ui_item->set_texture( UIUpgrade::LAYER_COLOR,  m_cell_textures[UIUpgrade::STATE_ENABLED].c_str() ); //default
	}
	
	m_scheme_wnd->Show	( true );
	m_item->Show		( true );
	m_back->Show		( true );

	UpdateAllUpgrades();
	return true;
}

UIUpgrade* CUIInventoryUpgradeWnd::FindUIUpgrade( Upgrade_type const* upgr )
{
	if ( !m_current_scheme )
	{
		return NULL;
	}
	UI_Upgrades_type::iterator ib = m_current_scheme->cells.begin();
	UI_Upgrades_type::iterator ie = m_current_scheme->cells.end();
	for ( ; ib != ie; ++ib )
	{
		Upgrade_type* i_upgr = (*ib)->get_upgrade();
		if ( upgr == i_upgr )
		{
			return (*ib);
		}
	}
	return NULL;
}

bool CUIInventoryUpgradeWnd::DBClickOnUIUpgrade( Upgrade_type const* upgr )
{
	UpdateAllUpgrades();
	UIUpgrade* uiupgr = FindUIUpgrade( upgr );
	if ( uiupgr )
	{
		uiupgr->OnClick();
		return true;
	}
	return false;
}

void CUIInventoryUpgradeWnd::AskUsing( LPCSTR text, LPCSTR upgrade_name )
{
	VERIFY( m_inv_item );
	VERIFY( upgrade_name );
	VERIFY( m_pParentWnd );

	UpdateAllUpgrades();

	m_cur_upgrade_id = upgrade_name;
	
	CUIActorMenu* parent_wnd = smart_cast<CUIActorMenu*>(m_pParentWnd);
	if ( parent_wnd )
	{
		parent_wnd->CallMessageBoxYesNo( text );
	}
}

void CUIInventoryUpgradeWnd::OnMesBoxYes()
{
	if ( get_manager().upgrade_install( *m_inv_item, m_cur_upgrade_id, false ) )
	{
		VERIFY( m_pParentWnd );
		CUIActorMenu* parent_wnd = smart_cast<CUIActorMenu*>( m_pParentWnd );
		if ( parent_wnd )
		{
			parent_wnd->UpdateActor();
			parent_wnd->SeparateUpgradeItem();
		}
	}
	UpdateAllUpgrades();
}

void CUIInventoryUpgradeWnd::HighlightHierarchy( shared_str const& upgrade_id )
{
	UpdateAllUpgrades();
	get_manager().highlight_hierarchy( *m_inv_item, upgrade_id );
}

void CUIInventoryUpgradeWnd::ResetHighlight()
{
	UpdateAllUpgrades();
	get_manager().reset_highlight( *m_inv_item );
}

void CUIInventoryUpgradeWnd::set_info_cur_upgrade( Upgrade_type* upgrade )
{
	UIUpgrade* uiu = FindUIUpgrade( upgrade );
	if ( uiu )
	{
		if ( Device.dwTimeGlobal < uiu->FocusReceiveTime())
		{
			upgrade = NULL; // visible = false
		}
	}
	else
	{
		upgrade = NULL;
	}

	CUIActorMenu* parent_wnd = smart_cast<CUIActorMenu*>(m_pParentWnd);
	if ( parent_wnd )
	{
		if ( parent_wnd->SetInfoCurUpgrade( upgrade, m_inv_item ) )
		{
			UpdateAllUpgrades();
		}
	}
}

CUIInventoryUpgradeWnd::Manager_type& CUIInventoryUpgradeWnd::get_manager()
{
	return  ai().alife().inventory_upgrade_manager();
}


void CUIInventoryUpgradeWnd::LoadCellsBacks(CUIXml& uiXml)
{
	XML_NODE* stored_root = uiXml.GetLocalRoot();

	int cnt = uiXml.GetNodesNum("cell_states", 0, "state");

	XML_NODE* node = uiXml.NavigateToNode("cell_states", 0);
	uiXml.SetLocalRoot(node);
	for (int i_st = 0; i_st < cnt; ++i_st)
	{
		uiXml.SetLocalRoot(uiXml.NavigateToNode("state", i_st));

		LPCSTR type = uiXml.Read("type", 0, "");
		LPCSTR txr = uiXml.Read("back_texture", 0, NULL);
		LPCSTR txr2 = uiXml.Read("point_texture", 0, NULL);
		u32    color = CUIXmlInit::GetColor(uiXml, "item_color", 0, 0);
		LoadCellStates(type, txr, txr2, color);

		uiXml.SetLocalRoot(node);
	}
	uiXml.SetLocalRoot(stored_root);
}

void CUIInventoryUpgradeWnd::LoadCellStates(LPCSTR state_str, LPCSTR texture_name, LPCSTR texture_name2, u32 color)
{
	VERIFY(state_str    && xr_strcmp(state_str, ""));
	if (texture_name && !xr_strcmp(texture_name, ""))
	{
		texture_name = NULL;
	}
	if (texture_name2 && !xr_strcmp(texture_name2, ""))
	{
		texture_name2 = NULL;
	}

	SetCellState(SelectCellState(state_str), texture_name, texture_name2, color);
}

UIUpgrade::ViewState CUIInventoryUpgradeWnd::SelectCellState(LPCSTR state_str)
{
	if (!xr_strcmp(state_str, "enabled")) { return UIUpgrade::STATE_ENABLED; }
	if (!xr_strcmp(state_str, "highlight")) { return UIUpgrade::STATE_FOCUSED; }
	if (!xr_strcmp(state_str, "touched")) { return UIUpgrade::STATE_TOUCHED; }
	if (!xr_strcmp(state_str, "selected")) { return UIUpgrade::STATE_SELECTED; }
	if (!xr_strcmp(state_str, "unknown")) { return UIUpgrade::STATE_UNKNOWN; }

	if (!xr_strcmp(state_str, "disabled_parent")) { return UIUpgrade::STATE_DISABLED_PARENT; }
	if (!xr_strcmp(state_str, "disabled_group")) { return UIUpgrade::STATE_DISABLED_GROUP; }
	if (!xr_strcmp(state_str, "disabled_money")) { return UIUpgrade::STATE_DISABLED_PREC_MONEY; }
	if (!xr_strcmp(state_str, "disabled_quest")) { return UIUpgrade::STATE_DISABLED_PREC_QUEST; }

	if (!xr_strcmp(state_str, "disabled_highlight")) { return UIUpgrade::STATE_DISABLED_FOCUSED; }

	VERIFY_FORMAT(0, "Such UI upgrade state (%s) does not exist !", state_str);
	return UIUpgrade::STATE_UNKNOWN;
}

void CUIInventoryUpgradeWnd::SetCellState(UIUpgrade::ViewState state, LPCSTR texture_name, LPCSTR texture_name2, u32 color)
{
	m_cell_textures[state] = texture_name;
	m_point_textures[state] = texture_name2;
}

bool CUIInventoryUpgradeWnd::VerirfyCells()
{
	for (int i = 0; i < UIUpgrade::STATE_COUNT; ++i)
	{
		if (!m_cell_textures[i]._get()) return false;
	}
	return true;
}

void CUIInventoryUpgradeWnd::LoadSchemes(CUIXml& uiXml)
{
	XML_NODE* stored_root = uiXml.GetLocalRoot();

	XML_NODE* tmpl_root = uiXml.NavigateToNode("templates", 0);
	uiXml.SetLocalRoot(tmpl_root);

	Frect	t_cell_item;

	t_cell_item.x1 = uiXml.ReadAttribFlt("cell_item", 0, "x");
	t_cell_item.y1 = uiXml.ReadAttribFlt("cell_item", 0, "y");
	t_cell_item.x2 = t_cell_item.x1 + uiXml.ReadAttribFlt("cell_item", 0, "width")*(UI().is_widescreen() ? 0.8f : 1.0f);
	t_cell_item.y2 = t_cell_item.y1 + uiXml.ReadAttribFlt("cell_item", 0, "height");

	int tmpl_count = uiXml.GetNodesNum(tmpl_root, "template");
	for (int i_tmpl = 0; i_tmpl < tmpl_count; ++i_tmpl)
	{
		XML_NODE* tmpl_node = uiXml.NavigateToNode("template", i_tmpl);
		uiXml.SetLocalRoot(tmpl_node);

		Scheme* scheme = xr_new<Scheme>();
		scheme->cells.reserve(MAX_UI_UPGRADE_CELLS);

		LPCSTR name = uiXml.ReadAttrib(tmpl_node, "name", "");
		VERIFY(name && xr_strcmp(name, ""));
		scheme->name._set(name);

		int clm_count = uiXml.GetNodesNum(tmpl_node, "column");
		for (int i_clm = 0; i_clm < clm_count; ++i_clm)
		{
			XML_NODE* clm_node = uiXml.NavigateToNode("column", i_clm);
			uiXml.SetLocalRoot(clm_node);

			int cell_cnt = uiXml.GetNodesNum(clm_node, "cell");
			for (int i_cell = 0; i_cell < cell_cnt; ++i_cell)
			{
				UIUpgrade* item = xr_new<UIUpgrade>(this);
				item->load_from_xml(uiXml, i_clm, i_cell, t_cell_item);
				CUIUpgradePoint* item_point = xr_new<CUIUpgradePoint>(item);
				item_point->load_from_xml(uiXml, i_cell);
				item->attach_point(item_point);

				scheme->cells.push_back(item);
			}// for i_cell

			uiXml.SetLocalRoot(tmpl_node);
		}// for i_clm

		m_schemes.push_back(scheme);
		uiXml.SetLocalRoot(tmpl_root);
	}// for i_tmpl

	uiXml.SetLocalRoot(stored_root);
}

