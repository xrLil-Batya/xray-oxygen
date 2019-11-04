///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../xrCore/xrCore.h"
#include "../../SDK/include/imdexlib/fast_dynamic_cast.hpp"

#define CNEW(type)			new (xr_alloc<type>(1)) type
#define CDELETE(ptr)			xr_delete(ptr)
#define CFREE(ptr)			xr_free(ptr)
#define CMALLOC(size)		xr_malloc(size)
#define CALLOC(type, count)	xr_alloc<type>(count)

template <bool _is_pm, typename T>
struct cspecial_free
{
	IC void operator()(T* &ptr)
	{
		void*	_real_ptr = imdexlib::fast_dynamic_cast<void*>(ptr);
		ptr->~T();
		CFREE(_real_ptr);
	}
};

template <typename T>
struct cspecial_free<false, T>
{
	IC void operator()(T* &ptr)
	{
		ptr->~T();
		CFREE(ptr);
	}
};

template <class T>
IC	void cdelete(T* &ptr)
{
	if (ptr)
	{
		cspecial_free<is_polymorphic<T>::result, T>()(ptr);
		ptr = nullptr;
	}
}

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define ENGINE_API
#include "../../3rd-party/OPCODE/Opcode.h"