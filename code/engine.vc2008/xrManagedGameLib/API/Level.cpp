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
#include "../xrGame/ActorEffector.h"
#include "../xrGame/Actor.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"
#include "../xrGame/HUDManager.h"
#include "../xrGame/UIGame.h"
#include "../xrGame/postprocessanimator.h"
#include "../xrGame/relation_registry.h"
#include "../xrGame/ui/UIGameTutorial.h"

 
extern GAME_API bool g_bDisableAllInput;
extern GAME_API CUISequencer* g_tutorial;
extern GAME_API CUISequencer* g_tutorial2;


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
void XRay::Level::WeatherFX::set(::System::String^ str)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().SetWeatherFX(WetNameStr);
	}
}
void XRay::Level::StartWeatherFXfromTime(::System::String^ str, float time)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().StartWeatherFXFromTime(WetNameStr, time);
	}
}
bool XRay::Level::iSWfxPlaying()
{
	return (::Environment().IsWeatherFXPlaying());
}
float XRay::Level::WfxTime::get()
{
	return (::Environment().wfx_time);
}
void XRay::Level::StopWeatherFX()
{
	(::Environment().StopWeatherFX());
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
void XRay::Level::GameDifficulty::set(ESingleGameDifficulty dif)
{
	g_SingleGameDifficulty = (::ESingleGameDifficulty)u32(dif);
	Actor()->OnDifficultyChanged();
}
XRay::ESingleGameDifficulty XRay::Level::GameDifficulty::get()
{
	return (ESingleGameDifficulty)u32(g_SingleGameDifficulty);
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
// CClientSpawnManager нет в Managed, нужно писать ему класс
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
void XRay::Level::ShowWeapon(bool b)
{
	psHUD_Flags.set(HUD_WEAPON_RT2, b);
}
bool XRay::Level::isLevelPresent()
{
	return (!!g_pGameLevel);
}
XRay::MEnvironment^ XRay::Level::pEnvironment()
{	
	return	(%(MEnvironment()));    //(&Environment())
}
XRay::EnvDescriptor^ XRay::Level::CurrentEnvironment(MEnvironment^ self)
{
	return gcnew EnvDescriptor(::System::IntPtr());
}

void XRay::Level::DisableInput()
{
	g_bDisableAllInput = true;
#ifdef DEBUG
	Msg("input disabled");
#endif // #ifdef DEBUG
}
void XRay::Level::EnableInput()
{
	g_bDisableAllInput = false;
#ifdef DEBUG
	Msg("input enabled");
#endif // #ifdef DEBUG
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
float XRay::Level::AddCamEffector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
	CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
	e->SetType((ECamEffectorType)id);
	e->SetCyclic(cyclic);
	e->Start(fn);
	Actor()->Cameras().AddCamEffector(e);
	return						e->GetAnimatorLength();
}
float XRay::Level::AddCamEffector2(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func, float cam_fov)
{
	CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
	e->m_bAbsolutePositioning = true;
	e->m_fov = cam_fov;
	e->SetType((ECamEffectorType)id);
	e->SetCyclic(cyclic);
	e->Start(fn);
	Actor()->Cameras().AddCamEffector(e);
	return						e->GetAnimatorLength();
}
void XRay::Level::RemoveCamEffector(int id)
{
	Actor()->Cameras().RemoveCamEffector((ECamEffectorType)id);
}
float XRay::Level::GetSndVolume()
{
	return psSoundVFactor;
}
void XRay::Level::SetSndVolume(float v)
{
	psSoundVFactor = v;
	clamp(psSoundVFactor, 0.0f, 1.0f);
}
void XRay::Level::AddComplexEffector(LPCSTR section, int id)
{
	AddEffector(Actor(), id, section);
}
void XRay::Level::RemoveComplexEffector(int id)
{
	RemoveEffector(Actor(), id);
}
void XRay::Level::AddPPEffector(LPCSTR fn, int id, bool cyclic)
{
	CPostprocessAnimator* pp = xr_new<CPostprocessAnimator>(id, cyclic);
	pp->Load(fn);
	Actor()->Cameras().AddPPEffector(pp);
}
void XRay::Level::RemovePPEffector(int id)
{
	CPostprocessAnimator*	pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if (pp) pp->Stop(1.0f);

}
void XRay::Level::SetPPEffectorFactor(int id, float f, float f_sp)
{
	CPostprocessAnimator*	pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if (pp) pp->SetDesiredFactor(f, f_sp);
}
void XRay::Level::SetPPEffectorFactor2(int id, float f)
{
	CPostprocessAnimator*	pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if (pp) pp->SetCurrentFactor(f);
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

