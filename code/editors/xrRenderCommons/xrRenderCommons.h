#pragma once

#ifdef XRRENDER_COMMONS_EXPORTS
#	define XRRENDER_COMMONS_API __declspec(dllexport)
#else
#	define XRRENDER_COMMONS_API __declspec(dllimport)
#endif