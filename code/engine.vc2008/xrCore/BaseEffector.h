#pragma once
#include "xrDelegate\xrDelegate.h"

struct XRCORE_API SBaseEffector
{
	using CB_ON_B_REMOVE = xrDelegate<void()>;

	CB_ON_B_REMOVE m_on_b_remove_callback;
	virtual ~SBaseEffector() {}
};

enum EEffectorPPType 
{
	ppeNext = 0,
};