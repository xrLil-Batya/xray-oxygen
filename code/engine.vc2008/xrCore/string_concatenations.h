#pragma once

XRCORE_API char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2);
XRCORE_API char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3);
XRCORE_API char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4);
XRCORE_API char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5);
XRCORE_API char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5, const char* S6);

// warning: do not comment this macro, as stack overflow check is very light
// (consumes ~1% performance of STRCONCAT macro)
#define STRCONCAT_STACKOVERFLOW_CHECK

#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#define STRCONCAT(dest, ...) \
	do { \
	xray::core::detail::string_tupples	STRCONCAT_tupples_unique_identifier(__VA_ARGS__); \
	u32 STRCONCAT_buffer_size = STRCONCAT_tupples_unique_identifier.size(); \
	xray::core::detail::check_stack_overflow(STRCONCAT_buffer_size); \
	(dest) = (char*)_alloca(STRCONCAT_buffer_size); \
	STRCONCAT_tupples_unique_identifier.concat	(dest); \
	} while (0)

#else //#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#define STRCONCAT(dest, ...) \
	do { \
	xray::core::detail::string_tupples	STRCONCAT_tupples_unique_identifier(__VA_ARGS__); \
	(dest)		       = (char*)_alloca(STRCONCAT_tupples_unique_identifier.size()); \
	STRCONCAT_tupples_unique_identifier.concat	(dest); \
	} while (0)

#endif //#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#include "string_concatenations_inline.h"
