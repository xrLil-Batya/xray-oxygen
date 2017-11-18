////////////////////////////////////////////////////////////////////////////
//	Module 		: property_string_shared_str.hpp
//	Created 	: 19.12.2007
//  Modified 	: 19.12.2007
//	Author		: Dmitriy Iassenev
//	Description : string property for shared_str implementation class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "property_value.hpp"
#include "property_holder_include.hpp"

namespace editor {
	class engine;
} // namespace editor

public ref class property_string_shared_str : public property_value {
public:
							property_string_shared_str	(editor::engine* engine, shared_str& value);
	virtual					~property_string_shared_str	();
							!property_string_shared_str	();
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);

private:
	editor::engine* m_engine;
	shared_str*		m_value;
}; // ref class property_string_shared_str
