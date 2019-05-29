////////////////////////////////////////////////////////////////////////////
//	Module 		: editor_environment_ambients_sound_id.hpp
//	Created 	: 04.01.2008
//  Modified 	: 04.01.2008
//	Author		: Dmitriy Iassenev
//	Description : editor environment ambients sound identifier class
////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef INGAME_EDITOR
#include "../include/editor/property_holder.hpp"

namespace editor {

class property_holder_collection;

namespace environment {
	namespace sound_channels {
		class manager;
	} // namespace sound_channels

namespace ambients {

class sound_id :
	public editor::property_holder_holder {
public:
							sound_id		(sound_channels::manager const& manager, shared_str const& sound);
                            sound_id(const sound_id&) = delete;
                            sound_id& operator= (const sound_id&) = delete;
	virtual					~sound_id		();
			void			fill			(editor::property_holder_collection* collection);
	inline	shared_str const& id			() const { return m_id; }

private:
	typedef editor::property_holder			property_holder_type;

public:
	virtual	property_holder_type* object	();

private:
	LPCSTR const*  collection		();
	u32  			collection_size	();

private:
	property_holder_type*					m_property_holder;
	sound_channels::manager const&			m_manager;
	shared_str								m_id;
}; // class sound_id
} // namespace ambients
} // namespace environment
} // namespace editor

#endif // #ifdef INGAME_EDITOR
