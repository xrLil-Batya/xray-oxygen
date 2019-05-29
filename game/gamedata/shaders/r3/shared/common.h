//////////////////////////////////////////////////
//  All comments by Nivenhbro are preceded by !
/////////////////////////////////////////////////


#ifndef SHARED_COMMON_H
#define SHARED_COMMON_H

//	Used by VS
cbuffer	dynamic_transforms
{
	row_major float3x4		m_W;			// local2world
	//row_major float3x4		m_invW;			// world2local
	row_major float3x4		m_V;			// world2view
	row_major float3x4		m_invV;			// view2world
	row_major float4x4 		m_P;			// view2proj
	//row_major float4x4		m_invP;			// proj2view
	row_major float3x4		m_WV;			// local2world2view
	row_major float4x4 		m_VP;			// world2view2proj
	row_major float4x4 		m_WVP;			// local2world2view2proj
	//	Used by VS only
	float4			L_material;	// 0,0,0,mid
	float4          hemi_cube_pos_faces;
	float4          hemi_cube_neg_faces;
	float4 			dt_params;	//	Detail params
}

cbuffer	static_globals
{
	float4		timers;

	float4		fog_plane;
	float4		fog_params;		// x=near*(1/(far-near)), ?,?, w = -1/(far-near)
	float4		fog_color; //maybe we should remove this, color the fog differently. Save a constant buffer call.

	float4		L_ambient;		// L_ambient.w = skynbox-lerp-factor
	float3		L_sun_color;
	float3		L_sun_dir_w;
	float4		L_hemi_color;

	float3 		eye_position;
	float3		eye_direction;
	float3		eye_normal;

	float4 		pos_decompression_params;

	float4		screen_res;		// Screen resolution (x=width, y=height, z=1/width, w=1/height)
	float4		parallax;
	float4		ssao_params;
	// Глобальные параметры шейдеров --#SM+#--
	row_major float4x4	m_script_params; 
	float4		m_hud_params;	// zoom_rotate_factor, secondVP_zoom_factor, NULL, NULL
	float4		m_blender_mode;	// x\y = [0 - default, 1 - night vision, 2 - thermal vision]; x - основной вьюпорт, y - второй впьюпорт, z = ?, w = [0 - идёт рендер обычного объекта, 1 - идёт рендер детальных объектов (трава, мусор)]

	// Параметры, уникальные для разных моделей --#SM+#--
	row_major float4x4	m_obj_camo_data; 
	float4		m_obj_custom_data;
	float4		m_obj_generic_data;
}

// Активен-ли двойной рендер --#SM+#--
inline bool isSecondVPActive()
{
	return (m_blender_mode.z == 1.f);
}

// Возвращает 1.f, если сейчас идёт рендер второго вьюпорта --#SM+#--
inline bool isSecondVP()
{
	return m_blender_mode.z > 0.5f;
}

// Возвращает режим блендинга для текущего вьюпорта --#SM+#--
float blender_mode()
{
	float ret = m_blender_mode.x;
	
	if (isSecondVP() == true)
		ret = m_blender_mode.y;
		
	return ret;
}

// В данный момент рендерятся детальные элементы (трава, мусор) --#SM+#--
inline bool isDetailRender()
{
	return (m_blender_mode.w == 1.f);
}

// Включён термо-режим --#SM+#--
inline bool isThermalMode()
{
	return (blender_mode() == 2.f);
}



/*
//




uniform float4x4 	m_texgen;
//uniform float4x4 	mVPTexgen;
uniform float3		L_sun_dir_e;
*/

float 	calc_cyclic 	(float x)				
{
	float 	phase 	= 1/(2*3.141592653589f);
	float 	sqrt2	= 1.4142136f;
	float 	sqrt2m2	= 2.8284271f;
	float 	f 	= sqrt2m2*frac(x)-sqrt2;	// [-sqrt2 .. +sqrt2] !No changes made, but this controls the grass wave (which is violent if I must say)
	return 	f*f - 1.f;				// [-1     .. +1]
}

float2 	calc_xz_wave 	(float2 dir2D, float frac)		
{
	// Beizer
	float2  ctrl_A	= float2(0.f,		0.f	);
	float2 	ctrl_B	= float2(dir2D.x,	dir2D.y	);
	return  lerp	(ctrl_A, ctrl_B, frac);			//!This calculates tree wave. No changes made
}


#endif
