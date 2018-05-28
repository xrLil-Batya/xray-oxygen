#pragma once

//Copy of export macros, because we can't include xrCore.h in C files
#ifndef XRCORE_API
    #ifdef XRCORE_EXPORTS
    #	define XRCORE_API __declspec(dllexport)
    #else
    #	define XRCORE_API __declspec(dllimport)
    #endif
#endif

//Special copy of xr_malloc for C files
#ifdef __cplusplus
extern "C"
{
#endif
    XRCORE_API void*	xr_malloc_C(size_t size);
    XRCORE_API void	    xr_free_C(void* ptr);
    XRCORE_API void*	xr_realloc_C(void* ptr, size_t size);
#ifdef __cplusplus
}
#endif