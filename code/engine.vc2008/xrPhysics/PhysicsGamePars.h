#pragma once

extern XRPHYSICS_API float collide_volume_max;
extern XRPHYSICS_API float collide_volume_min;

struct XRPHYSICS_API EffectPars
{
	const static  float vel_cret_sound;
	const static float vel_cret_particles;
	const static float vel_cret_wallmark;
};

struct XRPHYSICS_API CharacterEffectPars
{
	const static  float vel_cret_sound;
	const static float vel_cret_particles;
	const static float vel_cret_wallmark;
};

void XRPHYSICS_API LoadPhysicsGameParams();