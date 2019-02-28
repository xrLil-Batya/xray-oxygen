#include "stdafx.h"
#include "particle_effect.h"

using namespace PAPI;

ParticleEffect::ParticleEffect(int mp)
{
	owner = 0;
	param = 0;
	b_cb = 0;
	d_cb = 0;
	p_count = 0;
	max_particles = mp;
	particles_allocated = mp;
	particles = xr_alloc<Particle>(max_particles);
}

ParticleEffect::~ParticleEffect()
{
	xr_free(particles);
}

int ParticleEffect::Resize(u32 max_count)
{
	// Reducing max.
	if (particles_allocated >= max_count)
	{
		max_particles = max_count;

		// May have to kill particles.
		if (p_count > max_particles)
			p_count = max_particles;

		return max_count;
	}

	// Allocate particles.
	Particle* new_particles = xr_alloc<Particle>(max_count);
	if (!new_particles)
	{
		// ERROR - Not enough memory. Just give all we've got.
		max_particles = particles_allocated;
		return max_particles;
	}

	std::memcpy(new_particles, particles, p_count * sizeof(Particle));
	xr_free(particles);
	particles = new_particles;

	max_particles = max_count;
	particles_allocated = max_count;
	return max_count;
}

void ParticleEffect::Remove(int i)
{
	if (p_count)
	{
		Particle& m = particles[i];

		if (d_cb)
			d_cb(owner, param, m, i);

		m = particles[--p_count]; // не менять правило удаления !!! (dependence ParticleGroup)
	}
}