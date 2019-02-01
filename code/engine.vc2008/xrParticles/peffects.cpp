#include "stdafx.h"
#include "particle_effect.h"

using namespace PAPI;

ParticleEffect::ParticleEffect(int mp)
{
	owner = 0;
	param = 0;
	b_cb = 0;
	d_cb = 0;
	max_particles = mp;
	particles_allocated = mp;
}

ParticleEffect::~ParticleEffect()
{
	particles.clear();
}

int ParticleEffect::Resize(u32 max_count)
{
	// Allocate particles.
	max_particles = max_count;
	particles_allocated = max_count;
	return (u32)particles.size();
}

void ParticleEffect::Remove(int i)
{
	if (particles.size())
	{
		if (d_cb)
			d_cb(owner, param, particles[i], i);

		particles.erase(particles.begin() + i);
	}
}