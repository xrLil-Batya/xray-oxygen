//---------------------------------------------------------------------------
#pragma once
#pragma warning(push)
#pragma warning(disable: 4311 4302 4312)
namespace PAPI
{
	// A effect of particles - Info and an array of Particles
	struct PARTICLES_API ParticleEffect
	{
		u32 p_count;				// Number of particles currently existing.
		u32 max_particles;			// Max particles allowed in effect.
		u32 particles_allocated;	// Actual allocated size.
		Particle* particles;				// Actually, num_particles in size
		OnBirthParticleCB  b_cb;
		OnDeadParticleCB d_cb;
		void* owner;
		u32 param;

	public:
		ParticleEffect(int mp);
		~ParticleEffect();

		int Resize(u32 max_count);
		void Remove(int i);

		inline bool Add(const pVector &pos, const pVector &posB, const pVector &size, const pVector &rot, const pVector &vel, u32 color,
			const float age = 0.0f, u16 frame = 0, u16 flags = 0)
		{
			if (p_count >= max_particles)
				return false;
			else
			{
				Particle& P = particles[p_count];
				P.pos = pos;
				P.posB = posB;
				P.size = size;
				P.rot.x = rot.x;
				P.vel = vel;
				P.color = color;
				P.age = age;
				P.frame = frame;
				P.flags.assign(flags);

				if (b_cb)
					b_cb(owner, param, P, p_count);

				++p_count;

				return true;
			}
		}
	};
};
#pragma warning(pop)