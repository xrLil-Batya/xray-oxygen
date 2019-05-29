////////////////////////////////////////////////////////////////////////////
//	Module 		: engine_impl.cpp
//	Created 	: 04.12.2007
//  Modified 	: 04.12.2007
//	Author		: Dmitriy Iassenev
//	Description : engine implementation class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifdef INGAME_EDITOR
#include "engine_impl.hpp"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "IGame_Persistent.h"
#include "IGame_Level.h"
#include "editor_environment_weathers_time.hpp"
#include "editor_environment_manager.hpp"
#include "editor_environment_weathers_manager.hpp"

ENGINE_API extern CConsole* Console;

using editor::property_holder;

engine_impl::engine_impl		() :
	m_input_receiver(xr_new<IInputReceiver>()),
	m_input_captured(false)
{
}

engine_impl::~engine_impl		()
{
	capture_input	(false);
	xr_delete		(m_input_receiver);
}

bool engine_impl::on_message	(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		LRESULT &result
	)
{
	return			(Device.on_message(hWnd, uMsg, wParam, lParam, result));
}

void engine_impl::on_idle		()
{
	Device.on_idle	();
}

void engine_impl::on_resize		()
{
	if (Console)
		Console->Execute("vid_restart");
}

void engine_impl::pause			(bool const &value)
{
	if (value == !!Device.Paused())
		return;

	Device.Pause						(value ? TRUE : FALSE, TRUE, TRUE, "editor query");
}

void engine_impl::capture_input	(bool const &value)
{
	if (value == m_input_captured)
		return;

	m_input_captured					= value;

	if (value)
		m_input_receiver->IR_Capture	();
	else
		m_input_receiver->IR_Release	();
}

void engine_impl::disconnect	()
{
	Console->Execute("quit");
}

void engine_impl::value					(LPCSTR value, shared_str& result)
{
	result			= value;
}

LPCSTR engine_impl::value				(shared_str const& value)
{
	return			(value.c_str());
}

void engine_impl::weather(LPCSTR value)
{
	if (!g_pGamePersistent)
		return;

	shared_str new_weather_id = value;
	CEnvironment& environment = Environment();
	if (environment.CurrentWeatherName._get() == new_weather_id._get())
		return;

	typedef CEnvironment::EnvsMap	EnvsMap;
	EnvsMap const& weathers = environment.WeatherCycles;
	EnvsMap::const_iterator i = weathers.find(value);
	if (i == weathers.end())
		return;

	float const game_time = Environment().GetGameTime();
	environment.SetWeather(value, true);

	if (g_pGameLevel)
	{
		// FX: Позволим редактору погоды выставить редактируемую погоду, даже если он не на уровне
		g_pGameLevel->SetEnvironmentGameTimeFactor(iFloor(game_time), environment.fTimeFactor);
	}

	Environment().SelectEnvs	(game_time);
}

LPCSTR engine_impl::weather			()
{
	if (!g_pGamePersistent)
		return		("");

	return			(Environment().GetWeather().c_str());
}

void engine_impl::current_weather_frame		(LPCSTR frame_id)
{
	if (!g_pGamePersistent)
		return;

	shared_str				new_frame_id = frame_id;

	CEnvironment&			environment = Environment();
	VERIFY					(environment.CurrentWeather);
	typedef CEnvironment::EnvVec	EnvVec;
	EnvVec const&			frames = *environment.CurrentWeather;
	EnvVec::const_iterator	i = frames.begin();
	EnvVec::const_iterator	e = frames.end();
	for ( ; i != e; ++i)
		if ((*i)->m_identifier._get() == new_frame_id._get()) {
			environment.Current[0]	= (*i);
			environment.Current[1]	= ((i + 1) == e) ? (*frames.begin()) : *(i + 1);
			bool			set_time = true;
			if (environment.Current[0]->exec_time < environment.Current[1]->exec_time) {
				if (environment.GetGameTime() > environment.Current[0]->exec_time)
					if (environment.GetGameTime() < environment.Current[1]->exec_time)
						set_time	= false;
			}
			else {
				if (environment.GetGameTime() > environment.Current[0]->exec_time)
					set_time	= false;
			}
			if (set_time) {
				environment.SetGameTime	((*i)->exec_time, environment.fTimeFactor);
				g_pGameLevel->SetEnvironmentGameTimeFactor	(iFloor((*i)->exec_time*1000.f), environment.fTimeFactor);
			}
			break;
		}
}

LPCSTR engine_impl::current_weather_frame	()
{
	if (!g_pGamePersistent)
		return				("");

	if (!Environment().Current[0])
		return				("");

	return					(Environment().Current[0]->m_identifier.c_str());
}

void engine_impl::track_frame				(float const& time)
{
	VERIFY					(time >= 0.f);
	VERIFY					(time <= 1.f);

	if (!g_pGameLevel)
		return;

	CEnvironment&			environment = Environment();
	if (!environment.Current[0])
		return;

	if (!environment.Current[1])
		return;

	float					start_time = environment.Current[0]->exec_time;
	float					stop_time = environment.Current[1]->exec_time;
	float					current_time;
	if (start_time < stop_time)
		current_time		= start_time + time*(stop_time - start_time);
	else {
		stop_time			+= 24.f*60.f*60.f;
		current_time		= start_time + time*(stop_time - start_time);
		if (current_time >= 24.f*60.f*60.f)
			current_time	-= 24.f*60.f*60.f;
	}
	environment.SetGameTime	(current_time, environment.fTimeFactor);
	g_pGameLevel->SetEnvironmentGameTimeFactor	(iFloor(current_time*1000.f), environment.fTimeFactor);
}

float engine_impl::track_frame				()
{
	if (!g_pGamePersistent)
		return				(0.f);

	CEnvironment&			environment = Environment();
	if (!environment.Current[0])
		return				(0.f);

	float					start_time = environment.Current[0]->exec_time;
	float					stop_time = environment.Current[1]->exec_time;
	float					current_time = Environment().GetGameTime();
	if (start_time >= stop_time) {
		if (current_time >= start_time)
			clamp			(current_time, start_time, 24.f*60.f*60.f);
		else
			clamp			(current_time, 0.f, stop_time);

		if (current_time <= stop_time)
			current_time	+= 24.f*60.f*60.f;

		stop_time			+= 24.f*60.f*60.f;
	}
	else
		clamp				(current_time, start_time, stop_time);

	VERIFY					(start_time < stop_time);
	return					((current_time - start_time)/(stop_time - start_time));
}

void engine_impl::track_weather			(float const& time)
{
	VERIFY (time < 24*60*60);

	bool paused = Environment().m_paused;

	Environment().m_paused = false;
	Environment().SetGameTime(time * 24 * 60 * 60, Environment().fTimeFactor);
	Environment().m_paused = true;
	Environment().SetGameTime(time * 24 * 60 * 60, Environment().fTimeFactor);
	
	Environment().m_paused = paused;

	float weight;
	Environment().Invalidate();
	Environment().Lerp(weight);
}

float engine_impl::track_weather			()
{
	return (Environment().GetGameTime()/(24*60*60));
}

property_holder* engine_impl::current_frame_property_holder	()
{
	CEnvironment& environment = Environment();
	if (!environment.Current[0])
		return				(0);

	return					(
		((editor::environment::weathers::time&)(
			*environment.Current[0]
		)).object()
	);
}

property_holder* engine_impl::blend_frame_property_holder	()
{
	CEnvironment&			environment = Environment();
	if (!environment.CurrentEnv)
		return				(0);

	return					(
		((editor::environment::weathers::time&)(
			*environment.CurrentEnv
		)).object()
	);
}

property_holder* engine_impl::target_frame_property_holder	()
{
	CEnvironment&			environment = Environment();
	if (!environment.Current[1])
		return				(0);

	return					(
		((editor::environment::weathers::time&)(
			*environment.Current[1]
		)).object()
	);
}

void engine_impl::weather_paused		(bool const &value)
{
	Environment().m_paused		= value;
}

bool engine_impl::weather_paused		()
{
	return					(Environment().m_paused);
}

void engine_impl::weather_time_factor	(float const &value_raw)
{
	float					value = value_raw;
	clamp					(value, .01f, 100000.f);
	
	if (g_pGameLevel)
		g_pGameLevel->SetEnvironmentGameTimeFactor	(iFloor(Environment().GetGameTime()*1000.f), value);

	Environment().fTimeFactor= value;
}

float engine_impl::weather_time_factor	()
{
	if (!g_pGamePersistent)
		return				(1.f);

	return					(Environment().fTimeFactor);
}

void engine_impl::save_weathers			()
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	manager.Save					();
}

bool engine_impl::save_time_frame		(char* buffer, u32 const& buffer_size)
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	return							(
		manager.weathers().save_current_blend(
			buffer,
			buffer_size
		)
	);
}

bool engine_impl::paste_current_time_frame	(char const* buffer, u32 const& buffer_size)
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	return							(
		manager.weathers().paste_current_time_frame(
			buffer,
			buffer_size
		)
	);
}

bool engine_impl::paste_target_time_frame	(char const* buffer, u32 const& buffer_size)
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	return							(
		manager.weathers().paste_target_time_frame(
			buffer,
			buffer_size
		)
	);
}

bool engine_impl::add_time_frame		(char const* buffer, u32 const& buffer_size)
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	return							(
		manager.weathers().add_time_frame	(
			buffer,
			buffer_size
		)
	);
}

char const*	engine_impl::weather_current_time	() const
{
	return	(Environment().CurrentEnv->m_identifier.c_str());
}

void engine_impl::weather_current_time			(char const* time)
{
	u32		hours, minutes, seconds;
	sscanf_s(time, "%d:%d:%d", &hours, &minutes, &seconds);

	CEnvironment& refEnvironment = Environment();
	bool paused = refEnvironment.m_paused;

	refEnvironment.m_paused = false;
	refEnvironment.SetGameTime(float(hours*60*60 + minutes*60 + seconds), refEnvironment.fTimeFactor);
	refEnvironment.m_paused = paused;

	float weight;
	refEnvironment.Invalidate	();
	refEnvironment.Lerp		(weight);
}

void engine_impl::reload_current_time_frame		()
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	manager.weathers().reload_current_time_frame();
}

void engine_impl::reload_target_time_frame		()
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);
	manager.weathers().reload_target_time_frame();
}

void engine_impl::reload_current_weather()
{
	CEnvironment&					environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);

	float const game_time = environment.GetGameTime();
	manager.weathers().reload_current_weather();
	g_pGameLevel->SetEnvironmentGameTimeFactor(iFloor(game_time), environment.fTimeFactor);
	environment.Current[0] = 0;
	environment.Current[1] = 0;
	environment.SelectEnvs(game_time);

	VERIFY(environment.Current[1]);
	if (environment.Current[1]->exec_time == game_time)
		environment.SelectEnvs(game_time + .1f);
}

void engine_impl::reload_weathers()
{
	CEnvironment& environment = Environment();
	editor::environment::manager&	manager = dynamic_cast<editor::environment::manager&>(environment);

	float const game_time = Environment().GetGameTime();
	manager.weathers().reload();
	g_pGameLevel->SetEnvironmentGameTimeFactor(iFloor(game_time), environment.fTimeFactor);

	environment.Current[0] = 0;
	environment.Current[1] = 0;
	environment.SelectEnvs(game_time);

	VERIFY(environment.Current[1]);
	if (environment.Current[1]->exec_time == game_time)
		environment.SelectEnvs(game_time + .1f);
}

#endif // #ifdef INGAME_EDITOR
