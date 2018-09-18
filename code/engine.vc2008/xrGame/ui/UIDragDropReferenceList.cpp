#include "stdafx.h"
#include "UIDragDropReferenceList.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UIStatic.h"
#include "../inventory.h"
#include "../inventoryOwner.h"
#include "../actor.h"
#include "../actor_defs.h"
#include "UIInventoryUtilities.h"
#include "../../xrEngine/xr_input.h"
#include "../UICursor.h"
#include "UICellItemFactory.h"
#include "../../FRayBuildConfig.hpp"

CUIDragDropReferenceList::CUIDragDropReferenceList()
{
	AddCallbackStr("cell_item_reference", WINDOW_LBUTTON_DB_CLICK, CUIWndCallback::void_function(this, &CUIDragDropReferenceList::OnItemDBClick));
}

CUIDragDropReferenceList::~CUIDragDropReferenceList()
{
}

void CUIDragDropReferenceList::Initialize()
{
    Fvector2 pos;
    Fvector2 size;
    size.set(m_container->CellSize().x, m_container->CellSize().y);

    // Positions for vertical belts, if it's enabled
    const Fvector2 ElemPos[4] =
    {
        { 0.0f, 0.0f },
        { (float)m_container->CellSize().x + m_container->CellsSpacing().x, 0.0f },
        { 0.0f, (float)m_container->CellSize().y + m_container->CellsSpacing().y },
        { (float)m_container->CellSize().x + m_container->CellsSpacing().x, (float)m_container->CellSize().y + m_container->CellsSpacing().y }
    };

    for (int i = 0; i < 4; ++i)
    {
        m_references.push_back(xr_new<CUIStatic>());
        CUIStatic* pCell = m_references.back();
        if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
        {
            pos.set(ElemPos[i]);
        }
        else
        {
            pos.set((m_container->CellSize().x + m_container->CellsSpacing().x)*i, 0);
        }
        pCell->SetAutoDelete(true);
        pCell->SetWndPos(pos);
        pCell->SetWndSize(size);
        AttachChild(pCell);
        pCell->SetWindowName("cell_item_reference");
        Register(pCell);
    }
}

void CUIDragDropReferenceList::SetItem(CUICellItem* itm)
{
	inherited::SetItem(itm);
}

void CUIDragDropReferenceList::SetItem(CUICellItem* itm, Fvector2 abs_pos)
{
	const Ivector2 dest_cell_pos = m_container->PickCell(abs_pos);
	if(m_container->ValidCell(dest_cell_pos) && m_container->IsRoomFree(dest_cell_pos,itm->GetGridSize()))
		SetItem(itm, dest_cell_pos);
	else
	{
		if(dest_cell_pos.x!=-1&&dest_cell_pos.y!=-1)
		{
			CUICellItem* old_itm = GetCellAt(dest_cell_pos).m_item;
			RemoveItem(old_itm, false);
			SetItem(itm, dest_cell_pos);
		}
	}
}
void CUIDragDropReferenceList::SetItem(CUICellItem* itm, Ivector2 cell_pos)
{
    CUIStatic* ref = ref = m_references[cell_pos.x];
    if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
    {
        ref = m_references[cell_pos.x + 2 * cell_pos.y];
    }

	ref->SetShader(itm->GetShader());
	ref->SetTextureRect(itm->GetTextureRect());
	ref->TextureOn();
	ref->SetTextureColor(color_rgba(255,255,255,255));
	ref->SetStretchTexture(true);

	CUICell& C = m_container->GetCellAt(cell_pos);
	if(C.m_item!=itm)
	{
		m_container->PlaceItemAtPos(itm, cell_pos);
		itm->SetWindowName("cell_item");
		Register(itm);
		itm->SetOwnerList(this);
	}
}

CUICellItem* CUIDragDropReferenceList::RemoveItem(CUICellItem* itm, bool force_root)
{
	Ivector2 vec2 = m_container->GetItemPos(itm);
	if(vec2.x != -1 && vec2.y != -1)
	{
        u8 index = u8(vec2.x);
        if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
        {
            index = u8(vec2.x + 2 * vec2.y);
        }
		xr_strcpy(ACTOR_DEFS::g_quick_use_slots[index], "");
		m_references[index]->SetTextureColor(color_rgba(255,255,255,0));
	}
	inherited::RemoveItem(itm, force_root);
	return NULL;
}

void CUIDragDropReferenceList::LoadItemTexture(LPCSTR section, Ivector2 cell_pos)
{
    CUIStatic* ref = m_references[cell_pos.x];
    if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
    {
        ref = m_references[cell_pos.x + 2 * cell_pos.y];
    }

	ref->SetShader(InventoryUtilities::GetEquipmentIconsShader());
	Frect texture_rect;
	texture_rect.x1	= pSettings->r_float(section, "inv_grid_x")		*INV_GRID_WIDTH;
	texture_rect.y1	= pSettings->r_float(section, "inv_grid_y")		*INV_GRID_HEIGHT;
	texture_rect.x2	= pSettings->r_float(section, "inv_grid_width")	*INV_GRID_WIDTH;
	texture_rect.y2	= pSettings->r_float(section, "inv_grid_height")*INV_GRID_HEIGHT;
	texture_rect.rb.add(texture_rect.lt);
	ref->SetTextureRect(texture_rect);
	ref->TextureOn();
	ref->SetTextureColor(color_rgba(255,255,255,255));
	ref->SetStretchTexture(true);
}

void CUIDragDropReferenceList::ReloadReferences(CInventoryOwner* pActor)
{
	if (!pActor)
		return;

	if(m_drag_item)
		DestroyDragItem();

	m_container->ClearAll(true);
	m_selected_item	= nullptr;
    u8 BeltCount = 4u;
    //#REFACTOR: Vertical belts initial implementation was maded by Vasyan(c), so it's ugly and stupid
    // It's use x as x,y
    // Need remake that, once I (Giperion) got time
    if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
    {
        BeltCount = m_container->CellsCapacity().x;
    }
	for (u8 i = 0; i < BeltCount; i++)
	{
		CUIStatic* ref = m_references[i];
		LPCSTR item_name = ACTOR_DEFS::g_quick_use_slots[i];
		Ivector2 vec;

        vec.set(i, 0);

        if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
        {
            // Consider that BeltCount always aligned by 2
            u8 BeltCountRows = sqrt<u8>(BeltCount);
            vec.set(i / BeltCountRows, i % BeltCountRows);
        }

		if (item_name && xr_strlen(item_name))
		{
			PIItem itm = pActor->inventory().GetAny(item_name);
			if (itm) 
			{
				SetItem(create_cell_item(itm), vec);
			}
			else 
			{
				LoadItemTexture(item_name, vec);
				ref->SetTextureColor(color_rgba(255, 255, 255, 100));
			}
		}
		else 
		{
			ref->SetTextureColor(color_rgba(255, 255, 255, 0));
		}
	}
}

void CUIDragDropReferenceList::OnItemDBClick(CUIWindow* w, void* pData)
{
	CUIStatic* ref = smart_cast<CUIStatic*>(w);
	ITEMS_REFERENCES_VEC_IT it = std::find(m_references.begin(), m_references.end(), ref);
	if(it != m_references.end())
	{
		u8 index = u8(it-m_references.begin());
		CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());

		if (actor && actor->inventory().GetAny(ACTOR_DEFS::g_quick_use_slots[index]))
		{
            Ivector2 ElemIndex;

            ElemIndex.set(index, 0);
            if (g_extraFeatures.is(GAME_EXTRA_VERTICAL_BELTS))
            {
                u8 IndexRows = sqrt<u8>(index);
                ElemIndex.set(index / IndexRows, index % IndexRows);
            }
            inherited::RemoveItem(GetCellAt(ElemIndex).m_item, false);
		}

		xr_strcpy(ACTOR_DEFS::g_quick_use_slots[index], "");
		(*it)->SetTextureColor(color_rgba(255,255,255,0));
	}
}

void CUIDragDropReferenceList::OnItemDrop(CUIWindow* w, void* pData)
{
	OnItemSelected(w, pData);
	CUICellItem* itm = smart_cast<CUICellItem*>(w);
	VERIFY(itm->OwnerList() == itm->OwnerList());

	if(m_f_item_drop && m_f_item_drop(itm))
	{
		DestroyDragItem();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= m_drag_item->BackList();
	if(old_owner && new_owner && old_owner!=new_owner)
	{
		inherited::OnItemDrop(w, pData);
		return;
	}

	DestroyDragItem();
}
