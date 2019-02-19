/// ExportOGF for X-Ray Engine
/// v1.1

#include "stdafx.h"
#pragma hdrstop
#include "bone.h"
#include "GameMtlLib.h"

bool CBone::ExportOGF(IWriter& F)
{
	// check valid
	R_ASSERT3(!shape.Valid(), "! Bone '%s' has invalid shape.",*Name());

    const SGameMtl* M = GMLib.GetMaterial(game_mtl.c_str());
	R_ASSERT3(!M, "! Bone '%s' has invalid game material.", *Name());
	if (M == nullptr) return false;
	R_ASSERT3(!M->Flags.is(SGameMtl::flDynamic), "! Bone '%s' has non-dynamic game material.",*Name());

    F.w_u32		(OGF_IKDATA_VERSION);
    F.w_stringZ	(game_mtl);	
    F.w			(&shape,sizeof(SBoneShape));

    IK_data.Export(F);

    F.w_fvector3(rest_rotate);
    F.w_fvector3(rest_offset);
    F.w_float	(mass);
    F.w_fvector3(center_of_mass);
    return true;
}
