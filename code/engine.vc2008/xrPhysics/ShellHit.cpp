#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::applyHit(const Fvector& pos, const Fvector& dir, float val, const u16 id, ALife::EHitType hit_type)
{
	if (id == u16(-1))
		return;//

#pragma	todo("Kosya to kosya:this code shold treat all hit types")
	if (!m_pKinematics)
	{
		applyImpulseTrace(pos, dir, val);
		return;
	}

	switch (hit_type)
	{
	case ALife::eHitTypeExplosion:
		ExplosionHit(pos, dir, val, id);
		break;
	default: applyImpulseTrace(pos, dir, val, id);
	}
}

void CPHShell::ExplosionHit(const Fvector& pos, const Fvector& dir, float val, const u16 id)
{
	if (!isActive())
		return;

	EnableObject(0);

	float impulse = val / _sqrt(_sqrt((float)elements.size()));

	for (CPHElement* pElement : elements)
	{
		//Fvector max_area_dir;
		u16 gn = pElement->CPHGeometryOwner::numberOfGeoms();
		float g_impulse = impulse / gn;
		for (u16 j = 0; j < gn; ++j)
		{
			Fvector r_dir, r_pos, r_box;
			const float rad = pElement->getRadius();

			r_box.set(rad, rad, rad);
			r_pos.random_point(r_box);
			r_dir.random_dir();

			if (!fis_zero(pos.magnitude(), EPS_L))
			{
				r_dir.mul(0.5f);
				r_dir.add(dir);
			}

			r_dir.normalize_safe();//safe???
			pElement->applyImpulseTrace(r_pos, r_dir, g_impulse, pElement->CPHGeometryOwner::Geom(j)->bone_id());
		}
	}
}