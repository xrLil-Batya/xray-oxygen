////////////////////////////////////////////////////////////////////////////
//	Module 		: property_property_container.hpp
//	Created 	: 11.12.2007
//  Modified 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Description : property container property implementation class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "property_value.hpp"
class property_holder;

public ref class property_property_container : public property_value {
public:
							property_property_container	(property_holder* object);
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);

private:
	property_holder*		m_object;
}; // ref class property_property_container