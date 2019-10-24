/////////////////////////////////////////////////
// Author : ForserX
// Desc   : bases templates for C++ classes
/////////////////////////////////////////////////
// Oxygen Engine 2.0 - 2016-2019
/////////////////////////////////////////////////
#pragma once

#define xr_interface __interface

class xr_interface TNonCopyable
{
	virtual			~TNonCopyable	() = default;
private:
					TNonCopyable	(const TNonCopyable &) = delete;
					TNonCopyable	&operator=(const TNonCopyable &) = delete;
};

#define TMakeSingleton(T)
	public:                             \
	T(T const&) = delete;               \
	void operator=(T const&) = delete;  \
	static T& GetInstance()             \
	{                                   \
		static T Instance;              \
		return Instance;                \
	}