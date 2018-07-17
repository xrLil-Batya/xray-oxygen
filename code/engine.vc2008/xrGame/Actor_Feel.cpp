#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "mercuryball.h"
#include "inventory.h"
#include "character_info.h"
#include "xr_level_controller.h"
#include "UsableScriptObject.h"
#include "customzone.h"
#include "../xrEngine/gamemtllib.h"
#include "ui/UIMainIngameWnd.h"
#include "UIGame.h"
#include "Grenade.h"
#include "WeaponRPG7.h"
#include "ExplosiveRocket.h"
#include "game_cl_base.h"
#include "Level.h"
#include "clsid_game.h"
#include "hudmanager.h"
#include "ZoneCampfire.h"
#include "../FrayBuildConfig.hpp"

#define PICKUP_INFO_COLOR 0xFFFFA916

void CActor::feel_touch_new				(CObject* O)
{
	CPhysicsShellHolder* sh=smart_cast<CPhysicsShellHolder*>(O);
	if(sh&&sh->character_physics_support()) m_feel_touch_characters++;

}

void CActor::feel_touch_delete	(CObject* O)
{
	CPhysicsShellHolder* sh=smart_cast<CPhysicsShellHolder*>(O);
	if(sh&&sh->character_physics_support()) m_feel_touch_characters--;
}

BOOL CActor::feel_touch_contact		(CObject *O)
{
	CInventoryItem	*item = smart_cast<CInventoryItem*>(O);
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(O);

	if (item && item->Useful() && !item->object().H_Parent()) 
		return TRUE;

	if(inventory_owner && inventory_owner != smart_cast<CInventoryOwner*>(this))
	{
		return TRUE;
	}

	CZoneCampfire* camp = smart_cast<CZoneCampfire*>(O);
	if (camp) return TRUE;

	return		(FALSE);
}

BOOL CActor::feel_touch_on_contact	(CObject *O)
{
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
		return	(TRUE);

	Fsphere		sphere;
	Center		(sphere.P);
	sphere.R	= 0.1f;
	if (custom_zone->inside(sphere))
		return	(TRUE);

	return		(FALSE);
}

ICF static BOOL info_trace_callback(collide::rq_result& result, LPVOID params)
{
	BOOL& bOverlaped	= *(BOOL*)params;
	if(result.O)
	{
		if (Level().CurrentEntity()==result.O)
		{ //ignore self-actor
			return			TRUE;
		}else
		{ //check obstacle flag
			if(result.O->spatial.type&STYPE_OBSTACLE)
				bOverlaped			= TRUE;

			return			TRUE;
		}
	}else
	{
		//ïîëó÷èòü òðåóãîëüíèê è óçíàòü åãî ìàòåðèàë
		CDB::TRI* T		= Level().ObjectSpace.GetStaticTris()+result.element;
		if (GMLib.GetMaterialByIdx(T->material)->Flags.is(SGameMtl::flPassable)) 
			return TRUE;
	}	
	bOverlaped			= TRUE;
	return				FALSE;
}

BOOL CActor::CanPickItem(const CFrustum& frustum, const Fvector& from, CObject* item)
{
	if (!item->getVisible())
		return FALSE;

	BOOL	bOverlaped = FALSE;
	Fvector dir, to;
	item->Center(to);
	float range = dir.sub(to, from).magnitude();
	if (range > 0.25f)
	{
		if (frustum.testSphere_dirty(to, item->Radius()))
		{
			dir.div(range);
			collide::ray_defs			RD(from, dir, range, CDB::OPT_CULL, collide::rqtBoth);
			VERIFY(!fis_zero(RD.dir.square_magnitude()));
			RQR.r_clear();
			Level().ObjectSpace.RayQuery(RQR, RD, (collide::rq_callback*)info_trace_callback, &bOverlaped, nullptr, item);
		}
	}
	return !bOverlaped;
}

#include "..\xrEngine\xr_input.h"
void CActor::PickupModeUpdate()
{
	feel_touch_update	(Position(), m_fPickupInfoRadius);
	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB|FRUSTUM_P_FAR);

    for (CObject* obj : feel_touch)
    {
        Fvector act_and_cam_pos = Level().CurrentControlEntity()->Position();
        act_and_cam_pos.y += CameraHeight();
        if (CanPickItem(frustum, act_and_cam_pos, obj))
            PickupInfoDraw(obj);
    }

    //#TEMP: !!!
	m_CapmfireWeLookingAt = nullptr;
	for (CObject* obj : feel_touch)
	{
		CZoneCampfire* camp = smart_cast<CZoneCampfire*>(obj);
		if (camp)
		{
			Fvector dir, to;
			camp->Center(to);
			dir.sub(to, Device.vCameraPosition);
			float dist = dir.magnitude();
			float range = dir.normalize().dotproduct(Device.vCameraDirection);
			if (dist < 1.6f && range > 0.95f)
			{
				m_CapmfireWeLookingAt = camp;
				m_sDefaultObjAction = m_CapmfireWeLookingAt->is_on() ? m_sCampfireExtinguishAction : m_sCampfireIgniteAction;
				return;
			}
		}
	}
}

#include "../xrEngine/CameraBase.h"
BOOL	g_b_COD_PickUpMode = TRUE;
void CActor::PickupModeUpdate_COD(bool bDoPickup)
{
	if (Level().CurrentViewEntity() != this || !g_b_COD_PickUpMode) return;
		
	if (!g_Alive()) 
	{
		GameUI()->UIMainIngameWnd->SetPickUpItem(NULL);
		return;
	}
	
	CFrustum						frustum;
	frustum.CreateFromMatrix		(Device.mFullTransform, FRUSTUM_P_LRTB|FRUSTUM_P_FAR);

	ISpatialResult.clear	();
	g_SpatialSpace->q_frustum		(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);
	Fvector act_and_cam_pos = Level().CurrentControlEntity()->Position();
	act_and_cam_pos.y    += CameraHeight();
	float maxlen					= 1000.0f;
	CInventoryItem* pNearestItem	= NULL;

	for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
	{
		ISpatial*		spatial	= ISpatialResult[o_it];
		CInventoryItem*	pIItem	= smart_cast<CInventoryItem*> (spatial->dcast_CObject        ());

		if (0 == pIItem)											continue;
		if (pIItem->object().H_Parent() != NULL)					continue;
		if (!pIItem->CanTake())										continue;
		if ( smart_cast<CExplosiveRocket*>( &pIItem->object() ) )	continue;

		CGrenade*	pGrenade	= smart_cast<CGrenade*> (spatial->dcast_CObject        ());
		if (pGrenade && !pGrenade->Useful())						continue;

		CMissile*	pMissile	= smart_cast<CMissile*> (spatial->dcast_CObject        ());
		if (pMissile && !pMissile->Useful())						continue;
		
		Fvector A, B, tmp; 
		pIItem->object().Center			(A);
		if (A.distance_to_sqr(Position())>4)						continue;

		tmp.sub(A, cam_Active()->vPosition);
		B.mad(cam_Active()->vPosition, cam_Active()->vDirection, tmp.dotproduct(cam_Active()->vDirection));
		float len = B.distance_to_sqr(A);
		if (len > 1)												continue;

		if (maxlen>len && !pIItem->object().getDestroy())
		{
			maxlen = len;
			pNearestItem = pIItem;
		};
	}

    auto GetNearestPickableItem = [this, &act_and_cam_pos](CInventoryItem* InPickableItem) -> CInventoryItem*
    {
        if (CGameObject* pNearestGameObject = InPickableItem->cast_game_object())
        {
            CFrustum					frustum;
            frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);
            if (!CanPickItem(frustum, act_and_cam_pos, &InPickableItem->object()))
            {
                return nullptr;
            }

            if (!pNearestGameObject->getVisible() || Level().m_feel_deny.is_object_denied(pNearestGameObject))
            {
                return nullptr;
            }

            return InPickableItem;
        }

        return nullptr;
    };

    CInventoryItem* ValidatedPickableItem = nullptr;
    if (pNearestItem != nullptr)
    {
        ValidatedPickableItem = GetNearestPickableItem(pNearestItem);
    }

    // Update HUD - show pickable item
	GameUI()->UIMainIngameWnd->SetPickUpItem(ValidatedPickableItem);

    // Do pickup if we allowed and have pickable item (and don't look at person)
	if (bDoPickup && ValidatedPickableItem && !m_pPersonWeLookingAt)
	{
        CUsableScriptObject*	pUsableObject = smart_cast<CUsableScriptObject*>(ValidatedPickableItem);
        if (pUsableObject && (!m_pUsableObject))
            pUsableObject->use(this);

        //подбирание объекта
        Game().SendPickUpEvent(ID(), ValidatedPickableItem->object().ID());
	}
};

#include "eatable_item.h"
#include "EliteDetector.h"
#include "AdvancedDetector.h"
#include "SimpleDetector.h"
#include "grenadelauncher.h"
#include "Scope.h"
#include "Silencer.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"
#include "pda.h"

void CActor::PickupInfoDraw(CObject* object)
{
	LPCSTR draw_str = NULL;
	
	CArtefact* artefact = smart_cast<CArtefact*>(object);
	CEatableItem* boost = smart_cast<CEatableItem*>(object);
	CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(object);
	CWeapon* weapon = smart_cast<CWeapon*>(object);
	CInventoryItem* item = smart_cast<CInventoryItem*>(object);
	CEliteDetector* edetect = smart_cast<CEliteDetector*>(object);
	CAdvancedDetector* adetect = smart_cast<CAdvancedDetector*>(object);
	CSimpleDetector* sdetect = smart_cast<CSimpleDetector*>(object);
	CGrenade* grenade = smart_cast<CGrenade*>(object);
	CGrenadeLauncher* grenadela = smart_cast<CGrenadeLauncher*>(object);
	CScope* scope = smart_cast<CScope*>(object);
	CSilencer* sil = smart_cast<CSilencer*>(object);
	CCustomOutfit* outf = smart_cast<CCustomOutfit*>(object);
	CHelmet* helm = smart_cast<CHelmet*>(object);
	CPda* doc = smart_cast<CPda*>(object);
	if(!item)		return;

	Fmatrix			res;
	res.mul			(Device.mFullTransform,object->XFORM());
	Fvector4		v_res;
	Fvector			shift;

	draw_str = item->NameItem();
	shift.set(0,0,0);

	res.transform(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	UI().Font().pFontLetterica16Russian->SetAligment	(CGameFont::alCenter);
	if (!psActorFlags.test(AF_COLORED_FEEL))
	UI().Font().pFontLetterica16Russian->SetColor		(PICKUP_INFO_COLOR);
	else
	{
	if(doc)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFD7A096);
	if(ammo)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFFA121);
	if(weapon)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFF6B42);
	if(boost)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFF8330);
	if(artefact)
	UI().Font().pFontLetterica16Russian->SetColor		(0xFF736FD5);
	if(edetect)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEBDD0B);
	if(adetect)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEBFFA1);
	if(sdetect)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEBFFC8);
	if(grenade)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFF6432);
	if(grenadela)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEB6E0A);		
	if(scope && !grenadela)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEB8C0A);
	if(sil && !scope && !grenadela)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFEB8C64);
	if(outf)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFF6464);
	if(helm && !outf)
	UI().Font().pFontLetterica18Russian->SetColor		(0xFFFF9178);
	}
	UI().Font().pFontLetterica18Russian->Out			(x,y,draw_str);
}

void CActor::feel_sound_new(CObject* who, int type, CSound_UserDataPtr user_data, const Fvector& Position, float power)
{
	if(who == this)
		m_snd_noise = std::max(m_snd_noise, power);
}

void CActor::Feel_Grenade_Update( float rad )
{
	// Find all nearest objects
	Fvector pos_actor;
	Center( pos_actor );

	q_nearest.clear();
	g_pGameLevel->ObjectSpace.GetNearest( q_nearest, pos_actor, rad, nullptr);

	// select only grenade
	for (auto it: q_nearest)
	{
		if (it->getDestroy())
			continue;					// Don't touch candidates for destroy

		CGrenade* grn = smart_cast<CGrenade*>(it);
		if((!grn || grn->Initiator() == ID() || grn->Useful()) || grn->time_from_begin_throw() < m_fFeelGrenadeTime)
			continue;

	}

	HUD().Update_GrenadeView( pos_actor );
}

