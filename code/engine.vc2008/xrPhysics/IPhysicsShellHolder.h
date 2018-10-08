#pragma once
class ICollisionDamageInfo;
class IDamageSource;

class IKinematics;
class IPhysicsShellEx;
class IPHCapture;
class IPhysicsShellHolder;
class CPHSoundPlayer;
class ICollisionDamageReceiver;
class ICollisionForm;
class ICollisionHitCallback
{
public:
	virtual	void call(IPhysicsShellHolder* obj, float min_cs, float max_cs, float &cs, float &hl, ICollisionDamageInfo* di) = 0;
	virtual	~ICollisionHitCallback() {}
};

enum	EDumpType
{
	base,
	poses,
	vis_geom,
	props,
	full,
	full_capped
};

class IPhysicsShellHolder
{
public:

	virtual	Fmatrix&					__stdcall	ObjectXFORM() = 0;
	virtual	Fvector&					__stdcall	ObjectPosition() = 0;
	virtual	LPCSTR						__stdcall	ObjectName()		const = 0;
	virtual	LPCSTR						__stdcall	ObjectNameVisual()		const = 0;
	virtual	LPCSTR						__stdcall	ObjectNameSect()		const = 0;
	virtual	bool						__stdcall	ObjectGetDestroy()		const = 0;
	virtual ICollisionHitCallback*		__stdcall	ObjectGetCollisionHitCallback() = 0;
	virtual	u16							__stdcall	ObjectID()		const = 0;
	virtual	ICollisionForm*				__stdcall	ObjectCollisionModel() = 0;

	virtual	IKinematics*				__stdcall	ObjectKinematics() = 0;
	virtual IDamageSource*				__stdcall	ObjectCastIDamageSource() = 0;
	virtual	void						__stdcall	ObjectProcessingDeactivate() = 0;
	virtual	void						__stdcall	ObjectProcessingActivate() = 0;
	virtual	void						__stdcall	ObjectSpatialMove() = 0;
	virtual	IPhysicsShellEx*&				__stdcall	ObjectPPhysicsShell() = 0;
	virtual	void						__stdcall	enable_notificate() = 0;
	virtual bool						__stdcall	has_parent_object() = 0;
	virtual	void						__stdcall	on_physics_disable() = 0;
	virtual	IPHCapture*					__stdcall	PHCapture() = 0;
	virtual	bool						__stdcall	IsInventoryItem() = 0;
	virtual	bool						__stdcall	IsActor() = 0;
	virtual bool						__stdcall	IsStalker() = 0;

	virtual	void						__stdcall	HideAllWeapons(bool v) = 0;
	virtual	void						__stdcall	MovementCollisionEnable(bool enable) = 0;
	virtual CPHSoundPlayer*				__stdcall	ObjectPhSoundPlayer() = 0;
	virtual	ICollisionDamageReceiver*	__stdcall	ObjectPhCollisionDamageReceiver() = 0;
	virtual	void						__stdcall BonceDamagerCallback(float &damage_factor) = 0;
	virtual	xr_string					__stdcall	dump(EDumpType type) const = 0;
};
