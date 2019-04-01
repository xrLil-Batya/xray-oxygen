#include "stdafx.h"
#include "Level.h"
#include "../xrCore/LocatorAPI.h"
#include "../xrServerEntities/script_storage.cpp"
#include "../xrServerEntities/script_storage_space.h"
#include "../xrServerEntities/xrServer_Objects_ALife_Monsters.h"
#include "../xrGame/patrol_path_storage.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_simulator_base.h"
#include "../xrGame/alife_time_manager.h"
#include "../xrGame/alife_object_registry.h"
#include "../xrGame/Actor.h"
#include "../xrGame/HUDManager.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"
#include "../xrGame/UIGame.h"
#include "../xrGame/relation_registry.h"
#include "../xrGame/ui/UIGameTutorial.h"
#include "../xrGame/ui/UIMainIngameWnd.h"
#include "../xrGame/ui/UIMotionIcon.h"

extern GAME_API CUISequencer* g_tutorial;
extern GAME_API CUISequencer* g_tutorial2;
extern GAME_API CUIMotionIcon* g_pMotionIcon;

System::UInt32 XRay::LevelGraph::LevelID::get()
{
	return ai().level_graph().level_id();
}

System::UInt32 XRay::LevelGraph::VertexCount::get()
{
	return  ai().level_graph().header().vertex_count();
}
System::String^ XRay::Level::LevelName::get()
{
	return gcnew ::System::String(::Level().name_translated().c_str());
}

System::String^ XRay::Level::Weather::get()
{
	return gcnew ::System::String(::Environment().GetWeather().c_str());
}

void XRay::Level::Weather::set(::System::String^ str)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().SetWeather(WetNameStr, false);
	}
}

float XRay::Level::WfxTime::get()
{
	return (::Environment().wfx_time);
}

void XRay::Level::TimeFactor::set(float time_factor)
{
	if (Device.editor())
		return;
	(::Level().Server->game->SetGameTimeFactor(time_factor));
}
float XRay::Level::TimeFactor::get()
{
	return (::Level().GetGameTimeFactor());
}

float XRay::Level::RainFactor::get()
{
	return (::Environment().CurrentEnv->rain_density);
}
System::UInt32	XRay::Level::VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance)
{
	direction.normalize_safe();
	direction.mul(max_distance);
	Fvector			start_position = ai().level_graph().vertex_position(level_vertex_id);
	Fvector			finish_position = Fvector(start_position).add(direction);
	u32				result = u32(-1);
	ai().level_graph().farthest_vertex_in_direction(level_vertex_id, start_position, finish_position, result, nullptr);
	return			(::System::UInt32)(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}
System::Numerics::Vector3^ XRay::Level::VertexPosition(u32 level_vertex_id)
{
	const Fvector& NativeVec = ai().level_graph().vertex_position(level_vertex_id);

	return gcnew ::System::Numerics::Vector3(NativeVec.x, NativeVec.y, NativeVec.z);
}
float XRay::Level::HighCoverInDirection(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().high_cover_in_direction(y, level_vertex_id));
}
float XRay::Level::LowCoverInDirection(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().low_cover_in_direction(y, level_vertex_id));
}
bool XRay::Level::ValidVertex(u32 level_vertex_id)
{
	return ai().level_graph().valid_vertex_id(level_vertex_id);
}
void XRay::Level::MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
	{
		ml->SetHint(text);
	}
}
void XRay::Level::MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
		ml->SetHint(text);

	ml->SetSerializable(true);
}
void XRay::Level::MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().GetMapLocation(spot_type, id));
	if (!ml)				return;
	ml->SetHint(text);
}
void XRay::Level::MapRemoveObjectSpot(u16 id, LPCSTR spot_type)
{
	(::Level().MapManager().RemoveMapLocation(spot_type, id));
}
bool XRay::Level::MapHasObjectSpot(u16 id, LPCSTR spot_type)
{
	return (::Level().MapManager().HasMapLocation(spot_type, id));
}
bool XRay::Level::PatrolPathExists(LPCSTR patrol_path)
{
	return		(!!ai().patrol_paths().path(patrol_path, true));
}
System::String^ XRay::Level::Name::get()
{
	return gcnew ::System::String(pNativeLevel->name().c_str());
}
void XRay::Level::PrefetchSnd(LPCSTR name)
{
	(::Level().PrefetchSound(name));
}
XRay::ClientSpawnManager^ XRay::Level::ClientSpawnMngr::get()
{
	return gcnew ClientSpawnManager(&(::Level().client_spawn_manager()));
}
void XRay::Level::AddDialogToRender(UIDialogWnd^ pDialog)
{	
	((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->AddDialogToRender((CUIWindow*)pDialog->GetNative().ToPointer());
}
void XRay::Level::RemoveDialogFromRender(UIDialogWnd^ pDialog)
{
	((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->RemoveDialogToRender((CUIWindow*)pDialog->GetNative().ToPointer());
}
XRay::PhysicsWorldScripted^ XRay::Level::physicsWorldScripted()
{
	return gcnew PhysicsWorldScripted(get_script_wrapper<cphysics_world_scripted>(*physics_world()));
}
void XRay::Level::HideIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->HideShownDialogs();
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
	}
}
void XRay::Level::HideIndicatorsSafe()
{
	if ((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->OnExternalHideIndicators();
	}
}
void XRay::Level::ShowIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(true);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(true);
	}
}

XRay::GameObject^ XRay::Level::ViewEntity::get()
{
	CGameObject* pGameObject = smart_cast<CGameObject*>(::Level().CurrentViewEntity());
	if (!pGameObject)
		return (nullptr);

	return gcnew GameObject(::System::IntPtr(pGameObject));
}

void XRay::Level::ViewEntity::set(XRay::GameObject^ go)
{
	CObject* o = static_cast<CObject*>(go->GetNativeObject().ToPointer());
	::Level().SetViewEntity(o);
}

bool XRay::Level::LevelPresent::get()
{
	return (!!g_pGameLevel);
}

/*
XRay::MEnvironment^ XRay::Level::pEnvironment()
{	
	return	(MEnvironment(Environment())); //    (&Environment()) // fix it
}
*/

XRay::EnvDescriptor^ XRay::Level::CurrentEnvironment(MEnvironment^ self)
{
	return gcnew EnvDescriptor(::System::IntPtr());
}

void XRay::Level::SpawnPhantom(const Fvector &position)
{
	::Level().spawn_item("m_phantom", position, u32(-1), u16(-1), false);
}

Fbox XRay::Level::GetBoundingVolume()
{
	return ::Level().ObjectSpace.GetBoundingVolume();
}

void XRay::Level::IterateSounds(LPCSTR prefix, u32 max_count, CallBack callback)
{
	for (int j = 0, N = _GetItemCount(prefix); j < N; ++j) {
		string_path					fn, s;
		LPSTR						S = (LPSTR)&s;
		_GetItem(prefix, j, s);
		if (FS.exist(fn, "$game_sounds$", S, ".ogg"))
			callback(gcnew ::System::String(prefix));

		for (u32 i = 0; i < max_count; ++i)
		{
			string_path					name;
			xr_sprintf(name, "%s%d", S, i);
			if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
				callback(gcnew ::System::String(name));
		}
	}
}

int XRay::Level::GCommunityGoodwill(LPCSTR _community, int _entity_id)
{
	CHARACTER_COMMUNITY c;
	c.set(_community);

	return RELATION_REGISTRY().GetCommunityGoodwill(c.index(), u16(_entity_id));
}
void XRay::Level::GSetCommunityGoodwill(LPCSTR _community, int _entity_id, int val)
{
	CHARACTER_COMMUNITY	c;
	c.set(_community);
	RELATION_REGISTRY().SetCommunityGoodwill(c.index(), u16(_entity_id), val);
}
void XRay::Level::GChangeCommunityGoodwill(LPCSTR _community, int _entity_id, int val)
{
	CHARACTER_COMMUNITY	c;
	c.set(_community);
	RELATION_REGISTRY().ChangeCommunityGoodwill(c.index(), u16(_entity_id), val);
}
int XRay::Level::GGetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to)
{
	CHARACTER_COMMUNITY	community_from;
	community_from.set(comm_from);
	CHARACTER_COMMUNITY	community_to;
	community_to.set(comm_to);

	return RELATION_REGISTRY().GetCommunityRelation(community_from.index(), community_to.index());
}
void XRay::Level::GSetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to, int value)
{
	CHARACTER_COMMUNITY	community_from;
	community_from.set(comm_from);
	CHARACTER_COMMUNITY	community_to;
	community_to.set(comm_to);

	RELATION_REGISTRY().SetCommunityRelation(community_from.index(), community_to.index(), value);
}
int XRay::Level::GGetGeneralGoodwillBetween(u16 from, u16 to)
{
	CHARACTER_GOODWILL presonal_goodwill = RELATION_REGISTRY().GetGoodwill(from, to); VERIFY(presonal_goodwill != NO_GOODWILL);

	CSE_ALifeTraderAbstract* from_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(from));
	CSE_ALifeTraderAbstract* to_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(to));

	if (!from_obj || !to_obj) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "RELATION_REGISTRY::get_general_goodwill_between  : cannot convert obj to CSE_ALifeTraderAbstract!");
		return (0);
	}
	CHARACTER_GOODWILL community_to_obj_goodwill = RELATION_REGISTRY().GetCommunityGoodwill(from_obj->Community(), to);
	CHARACTER_GOODWILL community_to_community_goodwill = RELATION_REGISTRY().GetCommunityRelation(from_obj->Community(), to_obj->Community());

	return presonal_goodwill + community_to_obj_goodwill + community_to_community_goodwill;
}
::System::UInt32 XRay::LevelGraph::GetVertexId(Fvector position)
{
	return	(ai().level_graph().vertex_id(position));
}
void XRay::Level::StartTutorial(LPCSTR name)
{
	if (g_tutorial) {
		VERIFY(!g_tutorial2);
		g_tutorial2 = g_tutorial;
	};

	g_tutorial = xr_new<CUISequencer>();
	g_tutorial->Start(name);
	if (g_tutorial2)
		g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;

}
void XRay::Level::StopTutorial()
{
	if (g_tutorial)
		g_tutorial->Stop();
}
::System::String^ XRay::Level::TranslateString(LPCSTR str)
{
	return gcnew ::System::String(str);
}
bool XRay::Level::HasActiveTutotial()
{
	return (g_tutorial != nullptr);
}
void XRay::Level::GSend(NET_Packet& P)
{
	::Level().Send(P);
}
void XRay::Level::UEventGen(NET_Packet& P, u32 _event, u32 _dest)
{
	CGameObject::u_EventGen(P, _event, _dest);
}
void XRay::Level::UEventSend(NET_Packet& P)
{
	CGameObject::u_EventSend(P);
}
void SpawnSection(LPCSTR sSection, Fvector3 vPosition, u32 LevelVertexID, u16 ParentID, bool bReturnItem = false)
{
	Level().spawn_item(sSection, vPosition, LevelVertexID, ParentID, bReturnItem);
}
void ShowMinimap(bool bShow)
{
	CUIGame* GameUI = HUD().GetGameUI();
	GameUI->UIMainIngameWnd->ShowZoneMap(bShow);
	if (g_pMotionIcon != nullptr)
	{
		g_pMotionIcon->bVisible = bShow;
	}
}
XRay::ScriptGameObject^ XRay::Level::GGetTargetObject()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.O)
	{
		CGameObject	*game_object = static_cast<CGameObject*>(RQ.O);
		if (game_object)
			return gcnew ScriptGameObject(game_object->lua_game_object());
	}
	return (nullptr);
}
float XRay::Level::GGetTargetDist()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.range)
		return RQ.range;
	return (0);
}
::System::UInt32 XRay::Level::GGetTargetElement()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.element)
	{
		return RQ.element;
	}
	return (0);
}
u8 XRay::Level::GetActiveCam()
{
	CActor* actor = smart_cast<CActor*>(::Level().CurrentViewEntity());
	if (actor)
		return (u8)actor->active_cam();

	return 255;
}
void XRay::Level::SetActiveCam(u8 mode)
{
	CActor* actor = smart_cast<CActor*>(::Level().CurrentViewEntity());
	if (actor && mode <= EActorCameras::eacMaxCam)
		actor->cam_Set((EActorCameras)mode);
}

void XRay::Level::PatrolPathAdd(LPCSTR patrol_path, CPatrolPath* path) 
{
	ai().patrol_paths_raw().add_path(shared_str(patrol_path), path);
}

void XRay::Level::PatrolPathRemove(LPCSTR patrol_path)
{
	ai().patrol_paths_raw().remove_path(shared_str(patrol_path));
}

void XRay::Level::SpawnSection(LPCSTR sSection, Fvector3 vPosition, u32 LevelVertexID, u16 ParentID, bool bReturnItem)
{
	::Level().spawn_item(sSection, vPosition, LevelVertexID, ParentID, bReturnItem);
}

void XRay::Level::ShowMinimap(bool bShow)
{
	CUIGame* GameUI = HUD().GetGameUI();
	GameUI->UIMainIngameWnd->ShowZoneMap(bShow);
	if (g_pMotionIcon != nullptr)
	{
		g_pMotionIcon->bVisible = bShow;
	}
}
