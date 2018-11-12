////////////////////////////////////////////////////////////////////////////
//	Module 		: editor_environment_thunderbolts_thunderbolt_id.hpp
//	Created 	: 04.01.2008
//  Modified 	: 04.01.2008
//	Author		: Dmitriy Iassenev
//	Description : editor environment thunderbolts thunderbolt identifier class
////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef INGAME_EDITOR

#include "../include/editor/property_holder.hpp"

namespace editor {

class property_holder_collection;

namespace environment {
namespace thunderbolts {

class manager;

class thunderbolt_id :
	public editor::property_holder_holder {
public:
							thunderbolt_id	(manager const& manager, shared_str const& thunderbolt);
                            thunderbolt_id(const thunderbolt_id&) = delete;
                            thunderbolt_id& operator= (const thunderbolt_id&) = delete;
	virtual					~thunderbolt_id	();
			void			fill			(editor::property_holder_collection* collection);
	inline	LPCSTR			id				() const { return m_id.c_str(); }

private:
	typedef editor::property_holder			property_holder_type;

public:
	virtual	property_holder_type* object	();

private:
	LPCSTR const*  collection		();
	u32  			collection_size	();

private:
	property_holder_type*	m_property_holder;
	manager const&			m_manager;
	shared_str				m_id;
}; // class thunderbolt_id
} // namespace thunderbolts
} // namespace environment
} // namespace editor

#endif // #ifdef INGAME_EDITOR
