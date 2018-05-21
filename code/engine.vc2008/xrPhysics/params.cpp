#include "stdafx.h"

#include "params.h"
float object_damage_factor = 1.f; //times increace damage from object collision
void LoadParams()
{
	if (!pSettings)
		return;

	object_damage_factor = pSettings->r_float("physics", "object_damage_factor");
	object_damage_factor *= object_damage_factor;
}