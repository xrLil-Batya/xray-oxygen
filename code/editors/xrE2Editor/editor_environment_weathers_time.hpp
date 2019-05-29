////////////////////////////////////////////////////////////////////////////
//	Module 		: editor_environment_weathers_time.hpp
//	Created 	: 12.01.2008
//  Modified 	: 12.01.2008
//	Author		: Dmitriy Iassenev
//	Description : editor environment weathers time class
////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef INGAME_EDITOR

#include "../include/editor/property_holder.hpp"
#include "environment.h"

namespace editor {
namespace environment {

class manager;

namespace weathers {

class weather;

class time :
	public CEnvDescriptorMixer,
	public editor::property_holder_holder
{
private:
	typedef CEnvDescriptorMixer			inherited;

public:
	typedef editor::property_holder		property_holder_type;

public:
										time						(
											editor::environment::manager* manager,
											weather const* weather,
											shared_str const& id
										);
                                        time(const time&) = delete;
                                        time& operator= (const time&) = delete;
	virtual								~time						();
			void						load						(CInifile& config);
			void						load_from					(shared_str const& id, CInifile& config, shared_str const& new_id);
			void						save						(CInifile& config);
			void						fill						(::editor::property_holder_collection* holder);
	inline	shared_str const&			id							() const { return m_identifier; }
	virtual	property_holder_type*		object						() { return m_property_holder; }
	virtual	void						lerp						(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);

private:
	LPCSTR const*				ambients_collection			();
	u32							ambients_collection_size	();
	LPCSTR const*				suns_collection				();
	u32							suns_collection_size		();
	LPCSTR const*				thunderbolts_collection		();
	u32							thunderbolts_collection_size();

private:
	LPCSTR						id_getter					() const;
	void						id_setter					(LPCSTR value);
	float						sun_altitude_getter			() const;
	void						sun_altitude_setter			(float value);
	float						sun_longitude_getter		() const;
	void						sun_longitude_setter		(float value);
	float						sky_rotation_getter			() const;
	void						sky_rotation_setter			(float value);
	float						wind_direction_getter		() const;
	void						wind_direction_setter		(float value);
	LPCSTR						ambient_getter				() const;
	void						ambient_setter				(LPCSTR value);
	LPCSTR						sun_getter					() const;
	void						sun_setter					(LPCSTR value);
	LPCSTR						thunderbolt_getter			() const;
	void						thunderbolt_setter			(LPCSTR value);
	LPCSTR						sky_texture_getter			() const;
	void						sky_texture_setter			(LPCSTR value);
	LPCSTR						clouds_texture_getter		() const;
	void						clouds_texture_setter		(LPCSTR value);

private:
	shared_str							m_ambient;
	shared_str							m_sun;
	shared_str							m_thunderbolt_collection;

private:
	editor::environment::manager&		m_manager;
	weather const*						m_weather;
	property_holder_type*				m_property_holder;
}; // class time
} // namespace weathers
} // namespace environment
} // namespace editor

#endif // #ifdef INGAME_EDITOR
