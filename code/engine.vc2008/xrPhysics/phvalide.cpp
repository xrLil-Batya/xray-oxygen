#include "stdafx.h"

#include "phvalide.h"
#include "mathutils.h"
#include "iphysicsshellholder.h"

#include "..\xrCore\dump_string.h"

#ifdef DEBUG
XRPHYSICS_API bool bDebugVerifyBoundaries = true;
#else
XRPHYSICS_API bool bDebugVerifyBoundaries = false;
#endif

extern Fbox	phBoundaries;

bool valid_pos(const Fvector &P)
{
	return valid_pos(P, phBoundaries);
}
const Fbox	&ph_boundaries()
{
	return phBoundaries;
}

xr_string dbg_valide_pos_string(const Fvector &pos, const Fbox &bounds, const IPhysicsShellHolder *obj, LPCSTR msg)
{
	return	xr_string(msg) + xr_string(make_string("\n pos: %s , seems to be invalid ", get_string(pos).c_str())) +
		xr_string (make_string("\n Level box: %s ", get_string(bounds).c_str())) +
		xr_string("\n object dump: \n") +
		(obj ? obj->dump(full) : xr_string(""));
}

xr_string dbg_valide_pos_string(const Fvector &pos, const IPhysicsShellHolder *obj, LPCSTR msg)
{
	return dbg_valide_pos_string(pos, phBoundaries, obj, msg);
}