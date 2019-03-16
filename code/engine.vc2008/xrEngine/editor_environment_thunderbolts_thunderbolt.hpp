////////////////////////////////////////////////////////////////////////////
//	Module 		: editor_environment_thunderbolts_thunderbolt.hpp
//	Created 	: 04.01.2008
//  Modified 	: 04.01.2008
//	Author		: Dmitriy Iassenev
//	Description : editor environment thunderbolts thunderbolt class
////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef INGAME_EDITOR

#include "../include/editor/property_holder.hpp"
#include "editor_environment_thunderbolts_gradient.hpp"
#include "ThunderboltDesc.h"

namespace editor {
namespace environment {

class manager;

namespace thunderbolts {

class manager;

class thunderbolt :
	public CThunderboltDesc,
	public editor::property_holder_holder
{
private:
	typedef CThunderboltDesc			inherited;

public:
							thunderbolt				(manager* manager, shared_str const& id);
                            thunderbolt(const thunderbolt&) = delete;
                            thunderbolt& operator= (const thunderbolt&) = delete;
	virtual					~thunderbolt			();
			void			load					(CInifile& config);
			void			save					(CInifile& config);
			void			fill					(::editor::environment::manager& environment, editor::property_holder_collection* collection);
	inline	LPCSTR			id						() const { return m_id.c_str(); }
	virtual	void			create_top_gradient		(CInifile& pIni, shared_str const& sect);
	virtual	void			create_center_gradient	(CInifile& pIni, shared_str const& sect);

private:
			LPCSTR  id_getter	() const;
			void    id_setter	(LPCSTR value);
private:
	typedef editor::property_holder		property_holder_type;

public:
	virtual	property_holder_type* object();

private:
	shared_str				m_id;
	manager&				m_manager;
	property_holder_type*	m_property_holder;

private:
	gradient*				m_center;
	gradient*				m_top;
	shared_str				m_color_animator;
	shared_str				m_lighting_model;
	shared_str				m_sound;
}; // class thunderbolt

} // namespace thunderbolts
} // namespace environment
} // namespace editor

#endif // #ifdef INGAME_EDITOR
