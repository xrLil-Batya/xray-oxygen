//---------------------------------------------------------------------------
#pragma once
#pragma warning(push)
#pragma warning(disable: 4311 4302 4312)
namespace PAPI
{
	// A effect of particles - Info and an array of Particles
	struct PARTICLES_API ParticleEffect
	{
		u32 max_particles;			// Max particles allowed in effect.
		u32 particles_allocated;	// Actual allocated size.
		xr_vector<Particle> particles;				// Actually, num_particles in size
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
			// Lock: 128 particles on current frame
			if (particles.size() >= max_particles)
				return false;

			Particle P;
			P.pos = pos;
			P.posB = posB;
			P.size = size;
			P.rot.x = rot.x;
			P.vel = vel;
			P.color = color;
			P.age = age;
			P.frame = frame;
			P.flags.assign(flags);

			particles.emplace_back(P);

			if (b_cb)
				b_cb(owner, param, P, (u32)particles.size() - 1);

			return true;
		}
	};
};
#pragma warning(pop)