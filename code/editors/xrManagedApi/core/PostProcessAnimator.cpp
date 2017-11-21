#include "Pch.hpp"
#include "PostProcessAnimator.hpp"

void CPostprocessAnimator::Stop(float sp)
{
	if (m_bStop)			return;
	m_bStop = true;
	VERIFY(_valid(sp));
	m_factor_speed = sp;
}

float CPostprocessAnimator::GetLength()
{
	float v = 0.0f;
	for (int a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
	{
		float t = m_Params[a]->get_length();
		v = std::max(t, v);
	}
	return v;
}

void CPostprocessAnimator::Update(float tm)
{
	for (u32 a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
		m_Params[a]->update(tm);
}

void CPostprocessAnimator::SetDesiredFactor(float f, float sp)
{
	m_dest_factor = f;
	m_factor_speed = sp;
	VERIFY(_valid(m_factor));
	VERIFY(_valid(m_dest_factor));
};

void CPostprocessAnimator::SetCurrentFactor(float f)
{
	m_factor = f;
	m_dest_factor = f;
	VERIFY(_valid(m_factor));
	VERIFY(_valid(m_dest_factor));
};

BOOL CPostprocessAnimator::Process(float dt, SPPInfo &PPInfo)
{
	Update(dt);
	clamp(m_factor, 0.001f, 1.0f);

	PPInfo = m_EffectorParams;

	return TRUE;
}

void CPostprocessAnimator::Create()
{
	m_factor = 1.0f;
	m_dest_factor = 1.0f;
	m_bStop = false;
	m_start_time = -1.0f;
	m_factor_speed = 1.0f;
	f_length = 0.0f;

	m_Params[0] = xr_new<CPostProcessColor>(&m_EffectorParams.color_base);			//base color
	VERIFY(m_Params[0]);
	m_Params[1] = xr_new<CPostProcessColor>(&m_EffectorParams.color_add);          //add color
	VERIFY(m_Params[1]);
	m_Params[2] = xr_new<CPostProcessColor>(&m_EffectorParams.color_gray);         //gray color
	VERIFY(m_Params[2]);
	m_Params[3] = xr_new<CPostProcessValue>(&m_EffectorParams.gray);              //gray value
	VERIFY(m_Params[3]);
	m_Params[4] = xr_new<CPostProcessValue>(&m_EffectorParams.blur);              //blur value
	VERIFY(m_Params[4]);
	m_Params[5] = xr_new<CPostProcessValue>(&m_EffectorParams.duality.h);          //duality horizontal
	VERIFY(m_Params[5]);
	m_Params[6] = xr_new<CPostProcessValue>(&m_EffectorParams.duality.v);          //duality vertical
	VERIFY(m_Params[6]);
	m_Params[7] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.intensity);    //noise intensity
	VERIFY(m_Params[7]);
	m_Params[8] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.grain);        //noise granularity
	VERIFY(m_Params[8]);
	m_Params[9] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.fps);          //noise fps
	VERIFY(m_Params[9]);
}

CPostProcessParam* CPostprocessAnimator::GetParam(pp_params param)
{
	VERIFY(param >= pp_base_color && param <= pp_noise_f);
	return m_Params[param];
}
void CPostprocessAnimator::Save(LPCSTR name)
{
	IWriter *W = FS.w_open(name);
	VERIFY(W);
	W->w_u32(POSTPROCESS_FILE_VERSION);
	m_Params[0]->save(*W);
	m_Params[1]->save(*W);
	m_Params[2]->save(*W);
	m_Params[3]->save(*W);
	m_Params[4]->save(*W);
	m_Params[5]->save(*W);
	m_Params[6]->save(*W);
	m_Params[7]->save(*W);
	m_Params[8]->save(*W);
	m_Params[9]->save(*W);
	FS.w_close(W);
}
//-----------------------------------------------------------------------
void CPostprocessAnimator::ResetParam(pp_params param)
{
	xr_delete(m_Params[param]);
	switch (param)
	{
	case pp_base_color:
		m_Params[0] = xr_new<CPostProcessColor>(&m_EffectorParams.color_base);   //base color
		break;
	case pp_add_color:
		m_Params[1] = xr_new<CPostProcessColor>(&m_EffectorParams.color_add);          //add color
		break;
	case pp_gray_color:
		m_Params[2] = xr_new<CPostProcessColor>(&m_EffectorParams.color_gray);         //gray color
		break;
	case pp_gray_value:
		m_Params[3] = xr_new<CPostProcessValue>(&m_EffectorParams.gray);              //gray value
		break;
	case pp_blur:
		m_Params[4] = xr_new<CPostProcessValue>(&m_EffectorParams.blur);              //blur value
		break;
	case pp_dual_h:
		m_Params[5] = xr_new<CPostProcessValue>(&m_EffectorParams.duality.h);       //duality horizontal
		break;
	case pp_dual_v:
		m_Params[6] = xr_new<CPostProcessValue>(&m_EffectorParams.duality.v);       //duality vertical
		break;
	case pp_noise_i:
		m_Params[7] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.intensity);         //noise intensity
		break;
	case pp_noise_g:
		m_Params[8] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.grain);         //noise granularity
		break;
	case pp_noise_f:
		m_Params[9] = xr_new<CPostProcessValue>(&m_EffectorParams.noise.fps);         //noise fps
		break;
	}
	VERIFY(m_Params[param]);
}


CPostprocessAnimator::CPostprocessAnimator()
{
	Create();
}

CPostprocessAnimator::CPostprocessAnimator(int, bool)
{
	Create();
}

CPostprocessAnimator::~CPostprocessAnimator()
{
	Clear();
}


void CPostprocessAnimator::Clear()
{
	for (int a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
		xr_delete(m_Params[a]);
}

void CPostprocessAnimator::Load(LPCSTR name)
{
	m_Name = name;
	string_path full_path;
	xr_strcpy(full_path, name);

	LPCSTR  ext = strext(full_path);
	if (ext)
	{
		if (!xr_strcmp(ext, POSTPROCESS_FILE_EXTENSION))
		{
			IReader* F = FS.r_open(full_path);
			u32 dwVersion = F->r_u32();
			R_ASSERT(dwVersion == POSTPROCESS_FILE_VERSION);
			//load base color
			VERIFY(m_Params[0]);
			m_Params[0]->load(*F);
			//load add color
			VERIFY(m_Params[1]);
			m_Params[1]->load(*F);
			//load gray color
			VERIFY(m_Params[2]);
			m_Params[2]->load(*F);
			//load gray value
			VERIFY(m_Params[3]);
			m_Params[3]->load(*F);
			//load blur value
			VERIFY(m_Params[4]);
			m_Params[4]->load(*F);
			//load duality horizontal
			VERIFY(m_Params[5]);
			m_Params[5]->load(*F);
			//load duality vertical
			VERIFY(m_Params[6]);
			m_Params[6]->load(*F);
			//load noise intensity
			VERIFY(m_Params[7]);
			m_Params[7]->load(*F);
			//load noise granularity
			VERIFY(m_Params[8]);
			m_Params[8]->load(*F);
			//load noise fps
			VERIFY(m_Params[9]);
			m_Params[9]->load(*F);
			//close reader
			FS.r_close(F);
		}
		else
			FATAL("ERROR: Can't support files with many animations set. Incorrect file.");
	}

	f_length = GetLength();
}
