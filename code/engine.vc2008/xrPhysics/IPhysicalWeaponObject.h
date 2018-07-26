#pragma once

// FX: Special for dead anims 
class IWeaponObject
{
public:
	virtual			~IWeaponObject	() = default;

	virtual	bool	IsZoomed		() const = 0;
	virtual bool	IsScopeAttached	() const = 0;
};

class IWeaponShotgun
{
public:
	virtual ~IWeaponShotgun() = default;
};

class IExplosive
{
public:
	virtual ~IExplosive() = default;
};