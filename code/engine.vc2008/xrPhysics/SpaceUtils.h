#pragma once
#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include "../../3rd-party/ode/src/collision_kernel.h"
#pragma warning(default:4995)
#pragma warning(default:4267)
IC void spatialParsFromDGeom(dGeomID d_space, Fvector& center, Fvector& AABB, float& radius)
{
	d_space->computeAABB();
	dReal* dAABB = d_space->aabb;
	center.set((dAABB[0] + dAABB[1]) / 2.f, (dAABB[2] + dAABB[3]) / 2.f, (dAABB[4] + dAABB[5]) / 2.f);
	AABB.set(dAABB[1] - center.x, dAABB[3] - center.y, dAABB[5] - center.z);
	radius = std::max(AABB.x, std::max(AABB.y, AABB.z));
}