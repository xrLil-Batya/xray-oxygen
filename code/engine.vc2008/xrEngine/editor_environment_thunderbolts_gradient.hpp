////////////////////////////////////////////////////////////////////////////
//	Module 		: editor_environment_thunderbolts_gradient.hpp
//	Created 	: 04.01.2008
//  Modified 	: 10.01.2008
//	Author		: Dmitriy Iassenev
//	Description : editor environment thunderbolts gradient class
////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef INGAME_EDITOR
#include "../include/editor/property_holder.hpp"
#include "ThunderboltDesc.h"

namespace editor {

class property_holder;

namespace environment {

class manager;

namespace thunderbolts {

class gradient :
	public CThunderboltDesc::SFlare
{
public:
						gradient		();
                        gradient(const gradient&) = delete;
                        gradient& operator= (const gradient&) = delete;
						~gradient		();
			void		load			(CInifile& config, shared_str const& section_id, LPCSTR prefix);
			void		save			(CInifile& config, shared_str const& section_id, LPCSTR prefix);
			void		fill			(
							::editor::environment::manager& environment,
							LPCSTR name,
							LPCSTR description,
							editor::property_holder& holder
						);

private:
	LPCSTR		shader_getter	() const;
	void		shader_setter	(LPCSTR	value);

	LPCSTR		texture_getter	() const;
	void		texture_setter	(LPCSTR	value);

private:
	property_holder*	m_property_holder;
}; // class gradient


} // namespace thunderbolts
} // namespace environment
} // namespace editor

#endif // #ifdef INGAME_EDITOR
