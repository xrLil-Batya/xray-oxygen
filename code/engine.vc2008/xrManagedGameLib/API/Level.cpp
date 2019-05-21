#include "stdafx.h"
#include "Level.h"

#include "xrGame\ai_space.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_simulator_base.h"
#include "../xrGame/alife_time_manager.h"
#include "../xrGame/Actor.h"
#include "../xrGame/HUDManager.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"
#include "../xrGame/UIGame.h"
#include "../xrGame/patrol_path_storage.h"
#include "../xrGame/ui/UIMainIngameWnd.h"
#include "../xrGame/physics_world_scripted.h"

System::UInt32 XRay::LevelGraph::LevelID::get()
{
	return ai().level_graph().level_id();
}
System::UInt32 XRay::LevelGraph::VertexCount::get()
{
	return  ai().level_graph().header().vertex_count();
}
void XRay::LevelGraph::PatrolPathAdd(LPCSTR patrol_path, CPatrolPath* path)
{
	ai().patrol_paths_raw().add_path(shared_str(patrol_path), path);
}
void XRay::LevelGraph::PatrolPathRemove(LPCSTR patrol_path)
{
	ai().patrol_paths_raw().remove_path(shared_str(patrol_path));
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

bool XRay::Level::PatrolPathExists(LPCSTR patrol_path)
{
	return		(!!ai().patrol_paths().path(patrol_path, true));
}
System::String^ XRay::Level::Name::get()
{
	return gcnew ::System::String(pNativeLevel->name().c_str());
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

::System::UInt32 XRay::LevelGraph::GetVertexId(Fvector position)
{
	return	(ai().level_graph().vertex_id(position));
}

void SpawnSection(LPCSTR sSection, Fvector3 vPosition, u32 LevelVertexID, u16 ParentID, bool bReturnItem = false)
{
	Level().spawn_item(sSection, vPosition, LevelVertexID, ParentID, bReturnItem);
}
