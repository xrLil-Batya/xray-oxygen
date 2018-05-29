#pragma once

namespace ai_dbg
{
	void   set_var	(const char* name, float	value);
	void   show_var	(const char* name);
	bool   get_var	(const char* name, float&	value);
	bool   get_var  (const char* name, unsigned&value);
	bool   get_var  (const char* name, bool&	value);

} // namespace ai_dbg