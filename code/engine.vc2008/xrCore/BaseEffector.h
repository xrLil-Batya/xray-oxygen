#pragma once

struct XRCORE_API SBaseEffector
{
	typedef fastdelegate::FastDelegate0<>		CB_ON_B_REMOVE;
	CB_ON_B_REMOVE				m_on_b_remove_callback;
	virtual ~SBaseEffector() {}
};

enum EEffectorPPType {
	ppeNext = 0,
};