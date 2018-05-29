#include "stdafx.h"
#include "PhysicsGamePars.h"

float collide_volume_max = 200.f; //max collide sound level
float collide_volume_min = 0.f; //min collide sound level

const float EffectPars::vel_cret_sound = 10.f;
const float EffectPars::vel_cret_particles = 15.f;
const float EffectPars::vel_cret_wallmark = 30.f;

const float CharacterEffectPars::vel_cret_sound = 20.f;
const float CharacterEffectPars::vel_cret_particles = 60.f;
const float CharacterEffectPars::vel_cret_wallmark = 100.f;

void LoadPhysicsGameParams()
{
	collide_volume_min = pSettings->r_float("sound", "snd_collide_min_volume");
	collide_volume_max = pSettings->r_float("sound", "snd_collide_max_volume");
}