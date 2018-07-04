#include "stdafx.h"
#include "death_anims.h"
#include "../Include/xrRender/KinematicsAnimated.h"

BOOL death_anim_debug = FALSE;

rnd_motion::rnd_motion()
{
}

rnd_motion*	rnd_motion::setup(IKinematicsAnimated* k, const char* s)
{
	VERIFY(k);
	VERIFY(s);

	const u16 nb = (u16)_GetItemCount(s);
	for (u16 i = 0; nb > i; ++i)
	{
		string64 n;
		_GetItem(s, i, n);
		MotionID m = k->LL_MotionID(n);
		VERIFY2(m.valid(), make_string("motion: %s not found!", n));
		motions.push_back(m);
	}
	return this;
}

MotionID	rnd_motion::motion()	const
{
	if (motions.empty())
		return MotionID();
	return motions[::Random.randI(0, motions.size())];
}

void	type_motion::clear()
{
	std::for_each(anims.begin(), anims.end(), vec_clear<rnd_motion>);
	anims.clear();
}

XRPHYSICS_API xr_token motion_dirs[] = {
		{ "front",	type_motion::front	},
		{ "back",	type_motion::back 	},
		{ "left",	type_motion::left 	},
		{ "right",	type_motion::right	},
		{ 0,						0	}
};

void type_motion::set_motion(IKinematicsAnimated* k, u16 id_motion, const char* dir_anim)
{
	anims[id_motion] = xr_new<rnd_motion>()->setup(k, dir_anim);
}

type_motion* type_motion::setup(IKinematicsAnimated* k, CInifile * ini, const char* section, const char* type)
{
	anims.resize(dirs_number, 0);
	if (ini->line_exist(section, type))
	{
		const char* line = ini->r_string(section, type);
		if (!line)
			return this;

		R_ASSERT(xr_strlen(line) < 1023);
		const int num = _GetItemCount(line, '/');
#ifdef	DEBUG
		if (death_anim_debug && num == 0)
			Msg("death anims: load: no setings in section %s for %s", section, type);
#endif
		for (int i = 0; num > i; ++i)
		{
			string1024 sdir_anim;
			set_motion(k, u16(i), _GetItem(line, i, sdir_anim, '/'));
#ifdef	DEBUG
			if (death_anim_debug)
				Msg("death anims: load: loaded %s from section %s for %s", sdir_anim, section, type);
#endif
		}
	}
#ifdef	DEBUG
	else if (death_anim_debug)
		Msg("death anims: load: no setings in section %s for %s", section, type);

#endif
	return this;
}

type_motion::~type_motion()
{
	clear();
	VERIFY(anims.empty());
}

death_anims::death_anims()
{
}

death_anims::~death_anims()
{
	clear();
	VERIFY(anims.empty());
}

void	death_anims::clear()
{
	std::for_each(anims.begin(), anims.end(), vec_clear<type_motion>);
	anims.clear();
}

MotionID	type_motion::motion(type_motion::edirection dr)const
{
	VERIFY(dr < not_definite);
	rnd_motion *rm = anims[dr];
	MotionID m;
	if (rm)
		m = rm->motion();
	return m;
}

MotionID death_anims::motion(CEntityAlive& ea, const SHit& H, float &angle) const
{
	angle = 0;
	if (anims.empty())
		return rnd_anims.motion();

	MotionID m;
	xr_vector<type_motion*>::const_iterator i = anims.begin(), e = anims.end();
	for (; e != i; ++i)
		if ((*i)->predicate(ea, H, m, angle) && m.valid())
			return m;

	angle = 0;
	return rnd_anims.motion();
}