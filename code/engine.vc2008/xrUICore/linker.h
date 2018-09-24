#pragma once
#ifdef UICORE_BUILD
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif