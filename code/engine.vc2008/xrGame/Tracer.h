#pragma once
#include "../xrUICore/ui_defs.h"

class CBulletManager;

class CTracer
{
	friend CBulletManager;
protected:
	ui_shader		sh_Tracer;
	xr_vector<u32>	m_aColors;
	float			m_circle_size_k;
public:
					CTracer	();
	void			Render	(const Fvector& pos, const Fvector& cntr, const Fvector& dir, float len, float width, u8 ClrID, float speed, bool bAct);
};