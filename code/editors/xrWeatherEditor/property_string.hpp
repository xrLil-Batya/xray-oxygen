////////////////////////////////////////////////////////////////////////////
//	Module 		: property_string.hpp
//	Created 	: 07.12.2007
//  Modified 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Description : string property implementation class
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "property_holder_include.hpp"

public ref class property_string : public IProperty {
public:
	typedef editor::property_holder::string_getter_type	string_getter_type;
	typedef editor::property_holder::string_setter_type	string_setter_type;

public:
							property_string	(
								string_getter_type const &getter,
								string_setter_type const &setter
							);
	virtual					~property_string();
							!property_string();
	virtual System::Object	^GetValue		();
	virtual void			SetValue		(System::Object ^object);

private:
	string_getter_type		*m_getter;
	string_setter_type		*m_setter;
}; // ref class property_string
