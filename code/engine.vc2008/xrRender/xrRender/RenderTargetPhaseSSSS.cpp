#include "stdafx.h"

// Screen Space Sun Shafts
void CRenderTarget::PhaseSSSS()
{
	u32 Offset = 0;
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	// Constants
	float intensity = g_pGamePersistent->Environment().CurrentEnv->m_fSunShaftsIntensity;

	Fvector4 params = { 0.0f, 0.0f, 0.0f, 0.0f };
	xr_unordered_map<LPCSTR, Fvector4*> consts;
	consts.insert(std::make_pair("ssss_params", &params));

	if (ps_r_sunshafts_mode == SS_SS_MANOWAR)
	{
		// Mask
		render_screen_quad(_w, _h, Offset, rt_SunShaftsMask, s_ssss_mrmnwar->E[0]);

		// Smoothed mask
		render_screen_quad(_w, _h, Offset, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[1]);

		// Pass 0
		params.x = ps_r_prop_ss_sample_step_phase0;
		params.y = ps_r_prop_ss_radius;
		render_screen_quad(_w, _h, Offset, rt_SunShaftsPass0, s_ssss_mrmnwar->E[2], false, &consts);

		// Pass 1
		params.x = ps_r_prop_ss_sample_step_phase1;
		render_screen_quad(_w, _h, Offset, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[3], false, &consts);

		// Combine
		params.x = intensity;
		params.y = ps_r_prop_ss_blend;
		render_screen_quad(_w, _h, Offset, s_ssss_mrmnwar->E[4], true, &consts);
	}
	else if (ps_r_sunshafts_mode == SS_SS_OGSE)
	{
		// ***MASK GENERATION***
		// In this pass generates geometry mask
		render_screen_quad(_w, _h, Offset, rt_sunshafts_0, s_ssss_ogse->E[0]);

		// ***FIRST PASS***
		// First blurring pass
		params.x = intensity;
		params.y = ps_r_ss_sunshafts_length;
		params.z = 1.0f;
		params.w = ps_r_ss_sunshafts_radius;
		render_screen_quad(_w, _h, Offset, rt_sunshafts_1, s_ssss_ogse->E[1], false, &consts);

		//***SECOND PASS***
		// Second blurring pass
		params.z = 0.7f;
		render_screen_quad(_w, _h, Offset, rt_sunshafts_0, s_ssss_ogse->E[2], false, &consts);

		//***THIRD PASS***
		// Third blurring pass
		params.z = 0.3f;
		render_screen_quad(_w, _h, Offset, rt_sunshafts_1, s_ssss_ogse->E[3], false, &consts);

		//***BLEND PASS***
		// Combining sunshafts texture and image for further processing
		params.z = 0.0f;
		render_screen_quad(_w, _h, Offset, s_ssss_ogse->E[4], true, &consts);
	}
}
