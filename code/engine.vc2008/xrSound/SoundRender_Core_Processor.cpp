#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"
#include "SoundRender_Source.h"
#include "../xrCDB/cl_intersect.h"

CSoundRender_Emitter*	CSoundRender_Core::i_play(ref_sound* S, bool _loop, float delay)
{
	VERIFY(!S->_p->feedback);
	CSoundRender_Emitter* E = new CSoundRender_Emitter();
	S->_p->feedback = E;
	E->start(S, _loop, delay);
	emitters.push_back(E);

	return E;
}

void CSoundRender_Core::update(const Fvector& P, const Fvector& D, const Fvector& N)
{
	if (0 == bReady)
		return;

	bLocked = true;
	const float freshTimestamp = Timer.GetElapsed_sec();
	lastDeltaTime = freshTimestamp - lastTimestamp;
	lastTimestamp = freshTimestamp;

	++lastUpdateFrame;

	// Get currently rendering emitters
	targetShouldUpdateLater.clear();

	// Firstly update emitters, which are now being rendered
	for (CSoundRender_Target* target : targets)
	{
		if (CSoundRender_Emitter* emitter = target->get_emitter())
		{
			emitter->update(lastDeltaTime);
			emitter->lastUpdateFrame = lastUpdateFrame;
			emitter = target->get_emitter();	// update can stop itself
			if (emitter != nullptr)
			{
				target->priority = emitter->priority();
				continue;
			}
		}
		target->priority = -1;
	}

	// Update emitters
	for (u32 it = 0; it < emitters.size(); ++it)
	{
		CSoundRender_Emitter* emitter = emitters[it];

		if (emitter->lastUpdateFrame != lastUpdateFrame)
		{
			emitter->update(lastDeltaTime);
			emitter->lastUpdateFrame = lastUpdateFrame;
		}

		if (!emitter->isPlaying())
		{
			// Stopped
			xr_delete(emitter);
			emitters.erase(emitters.begin() + it);
			it--;
		}
	}

	// Get currently rendering emitters
	targetShouldUpdateLater.clear();

	for (CSoundRender_Target* pTarget : targets)
	{
		if (pTarget->get_emitter())
		{
			// Has emitter, maybe just not started rendering
			if (pTarget->IsRendering())
			{
				pTarget->fill_parameters();
				pTarget->update();
			}
			else
			{
				targetShouldUpdateLater.push_back(pTarget);
			}
		}
	}

	// Commit parameters from pending targets
	if (!targetShouldUpdateLater.empty())
	{
		targetShouldUpdateLater.erase(std::unique(targetShouldUpdateLater.begin(), targetShouldUpdateLater.end()), targetShouldUpdateLater.end());
		for (CSoundRender_Target* target : targetShouldUpdateLater)
		{
			target->fill_parameters();
		}
	}

	// update EFX
	if (psSoundFlags.test(ss_EFX) && bEFX)
	{
		if (bListenerMoved)
		{
			bListenerMoved = false;
			e_target = *get_environment(P);
		}

		e_current.lerp(e_current, e_target, lastDeltaTime);
		// KRodin: Сделал по аналогии с eax. Некоторые эффекты подошли. Посмотрим, что получится.
		i_efx_listener_set(&e_current);
		bEFX = i_efx_commit_setting();
	}

	// update listener
	update_listener(P, D, N, lastDeltaTime);

	// Start rendering of pending targets
	for (CSoundRender_Target* target : targetShouldUpdateLater)
	{
		target->render();
	}

	// Events
	update_events();

	bLocked = false;
}

static	u32	g_saved_event_count = 0;

void CSoundRender_Core::update_events()
{
	g_saved_event_count = (u32)s_events.size();

	for (u32 it = 0; it < g_saved_event_count; ++it)
	{
		event&	E = s_events[it];
		Handler(E.first, E.second);
	}

	s_events.clear();
}

void CSoundRender_Core::statistic(CSound_stats*  dest, CSound_stats_ext*  ext)
{
	if (dest)
	{
		dest->_rendered = 0;
		for (u32 it = 0; it < targets.size(); ++it)
		{
			CSoundRender_Target* T = targets[it];
			if (T->get_emitter() && T->IsRendering())
				dest->_rendered++;
		}

		dest->_simulated = (u32)emitters.size();
		dest->_cache_hits = cache._stat_hit;
		dest->_cache_misses = cache._stat_miss;
		dest->_events = g_saved_event_count;
		cache.stats_clear();
	}

	if (ext)
	{
		for (u32 it = 0; it < emitters.size(); ++it)
		{
			CSoundRender_Emitter*	_E = emitters[it];
			CSound_stats_ext::SItem _I;
			_I._3D = !_E->b2D;
			_I._rendered = !!_E->target;
			_I.params = _E->p_source;
			_I.volume = _E->smooth_volume;

			if (_E->owner_data)
			{
				_I.name = _E->source()->fname;
				_I.game_object = _E->owner_data->g_object;
				_I.game_type = _E->owner_data->g_type;
				_I.type = _E->owner_data->s_type;
			}
			else
			{
				_I.game_object = 0;
				_I.game_type = 0;
				_I.type = st_Effect;
			}

			ext->append(_I);
		}
	}
}

float CSoundRender_Core::get_occlusion_to(const Fvector& hear_pt, const Fvector& snd_pt, float dispersion)
{
	float occ_value = 1.f;

	if (0 != geom_SOM)
	{
		// Calculate RAY params
		Fvector	pos, dir;
		pos.random_dir();
		pos.mul(dispersion);
		pos.add(snd_pt);
		dir.sub(pos, hear_pt);
		float range = dir.magnitude();
		dir.div(range);

		geom_DB.ray_options(CDB::OPT_CULL);
		geom_DB.ray_query(geom_SOM, hear_pt, dir, range);
		size_t r_cnt = geom_DB.r_count();

		if (0 != r_cnt)
		{
			for (u32 k = 0; k < r_cnt; ++k)
			{
				CDB::RESULT& triElement = geom_DB.r_getElement(k);
				occ_value *= *(float*)& triElement.dummy;
			}
		}
	}
	return occ_value;
}

float CSoundRender_Core::get_occlusion(Fvector& P, float R, Fvector* occ)
{
	float occ_value = 1.f;

	// Calculate RAY params
	Fvector base = listener_position();
	Fvector	pos, dir;
	float range;
	pos.random_dir();
	pos.mul(R);
	pos.add(P);
	dir.sub(pos, base);
	range = dir.magnitude();
	dir.div(range);

	if (0 != geom_MODEL)
	{
		bool bNeedFullTest = true;
		// 1. Check cached polygon
		float _u, _v, _range;
		if (CDB::TestRayTri(base, dir, occ, _u, _v, _range, true))
			if (_range > 0 && _range < range)
			{
				occ_value = psSoundOcclusionScale;
				bNeedFullTest = false;
			}

		// 2. Polygon doesn't picked up - real database query
		if (bNeedFullTest)
		{
			geom_DB.ray_options(CDB::OPT_ONLYNEAREST);
			geom_DB.ray_query(geom_MODEL, base, dir, range);
			if (0 != geom_DB.r_count())
			{
				// cache polygon
				const auto pR = geom_DB.r_realBegin();
				const CDB::TRI& T = geom_MODEL->get_tris()[pR->id];
				const Fvector* V = geom_MODEL->get_verts();
				occ[0].set(V[T.verts[0]]);
				occ[1].set(V[T.verts[1]]);
				occ[2].set(V[T.verts[2]]);
				occ_value = psSoundOcclusionScale;
			}
		}
	}

	if (0 != geom_SOM)
	{
		geom_DB.ray_options(CDB::OPT_CULL);
		geom_DB.ray_query(geom_SOM, base, dir, range);
		size_t r_cnt = geom_DB.r_count();

		if (0 != r_cnt)
		{
			for (u32 k = 0; k < r_cnt; ++k)
			{
				CDB::RESULT& pR = geom_DB.r_getElement(k);
				occ_value *= *(float*)&pR.dummy;
			}
		}
	}

	return occ_value;
}