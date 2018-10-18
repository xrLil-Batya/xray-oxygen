#pragma once

#ifdef MANAGED_UI_BUILD
#define MANAGED_UI_API __declspec(dllexport)
#else
#define MANAGED_UI_API __declspec(dllimport)
#endif