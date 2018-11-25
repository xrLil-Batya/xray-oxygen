////////////////////////////////////////////////////////////////////////////
//	Module 		: ide.hpp
//	Created 	: 04.12.2007
//  Modified 	: 04.12.2007
//	Author		: Dmitriy Iassenev
//	Description : IDE interface class
////////////////////////////////////////////////////////////////////////////
#pragma once
namespace editor 
{

class property_holder;
class property_holder_collection;
class property_holder_holder;

class ide {
public:
	virtual	HWND				main_handle				() = 0;
	virtual	HWND				view_handle				() = 0;
	virtual	void				run						() = 0;
	virtual	void				on_load_finished		() = 0;
	virtual	void				pause					() = 0;

public:
	virtual	property_holder*	create_property_holder	(LPCSTR display_name, property_holder_collection* collection = nullptr, property_holder_holder* holder = nullptr) = 0;
	virtual	void				destroy					(property_holder *&property_holder) = 0;
	virtual	void				environment_levels		(property_holder *property_holder) = 0;
	virtual	void				environment_weathers	(property_holder *property_holder) = 0;

public:
	using weathers_getter_type		= xrDelegate<LPCSTR const*()>;
	using weathers_size_getter_type = xrDelegate<u32()>;
	using frames_getter_type		= xrDelegate<LPCSTR const*(const char*)>;
	using frames_size_getter_type	= xrDelegate<u32(const char*)>;
	virtual	void weather_editor_setup(weathers_getter_type const& weathers_getter, weathers_size_getter_type const& weathers_size_getter, frames_getter_type const& frames_getter, frames_size_getter_type const& frames_size_getter) = 0;
}; // class ide

} // namespace editor