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
	particles.reserve(max_particles);
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

	// May have to kill particles.
	if (particles.size() > max_particles)
	{
		for (decltype(particles)::iterator IterSize = particles.begin() + max_count; IterSize <= particles.end(); IterSize++)
		{
			particles.erase(IterSize);
		}
	}

	return max_count;
}

void ParticleEffect::Remove(int i)
{
	if (particles.size())
	{
		Particle& m = particles[i];
		if (d_cb)
			d_cb(owner, param, m, i);

		m = particles[particles.size() - 1]; // не менять правило удаления !!! (dependence ParticleGroup)
		particles.pop_back();
	}
}