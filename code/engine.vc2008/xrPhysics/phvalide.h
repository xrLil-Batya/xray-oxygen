#pragma	once

extern XRPHYSICS_API bool bDebugVerifyBoundaries;

XRPHYSICS_API bool valid_pos(const Fvector &P);
XRPHYSICS_API const Fbox	&ph_boundaries();

class IPhysicsShellHolder;
XRPHYSICS_API xr_string dbg_valide_pos_string(const Fvector &pos, const Fbox &bounds, const IPhysicsShellHolder *obj, LPCSTR msg);
XRPHYSICS_API xr_string dbg_valide_pos_string(const Fvector &pos, const IPhysicsShellHolder *obj, LPCSTR msg);

#ifndef PVS_STUDIO
#define	VERIFY_BOUNDARIES2(pos,bounds,obj,msg) if (bDebugVerifyBoundaries) \
        VERIFY2(  valid_pos( pos, bounds ), dbg_valide_pos_string( pos, bounds, obj, msg ).c_str() )

#define	VERIFY_BOUNDARIES(pos,bounds,obj)	if (bDebugVerifyBoundaries) \
    VERIFY_BOUNDARIES2(pos,bounds,obj,"	")
#endif