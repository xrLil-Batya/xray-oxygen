#pragma once
class CBoneInstance;
class IKinematics;
struct XRPHYSICS_API anim_bone_fix
{
	CBoneInstance *bone;
	CBoneInstance *parent;
	Fmatrix		  matrix;

	anim_bone_fix();
	~anim_bone_fix();
	static	void	__stdcall	callback(CBoneInstance *BI);
	void fix(u16 bone_id, IKinematics &K);
	void refix();
	void release();
	void deinit();
};

bool XRPHYSICS_API find_in_parents(const u16 bone_to_find, const u16 from_bone, IKinematics &ca);