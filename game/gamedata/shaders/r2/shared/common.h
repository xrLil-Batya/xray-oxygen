//////////////////////////////////////////////////
//  All comments by Nivenhbro are preceded by !
/////////////////////////////////////////////////


#ifndef SHARED_COMMON_H
#define SHARED_COMMON_H
//
uniform float3x4	m_W;			// local2world
uniform float3x4	m_invW;			// world2local
uniform float3x4	m_V;			// world2view
uniform float3x4	m_invV;			// view2world
uniform float4x4 	m_P;			// view2proj
uniform float4x4	m_invP;			// proj2view
uniform float3x4	m_WV;			// local2world2view
uniform float4x4 	m_VP;			// world2view2proj
uniform float4x4 	m_WVP;			// local2world2view2proj

uniform float4x4 	m_texgen;
uniform float4x4 	mVPTexgen;

uniform float4		timers;
uniform float4		fog_plane;
uniform float4		fog_params;		// x=near*(1/(far-near)), ?,?, w = -1/(far-near)
uniform float4		fog_color;
uniform float3		L_sun_color;
uniform float3		L_sun_dir_w;
uniform float3		L_sun_dir_e;
uniform float4		L_hemi_color;
uniform float4		L_ambient;		// L_ambient.w = skynbox-lerp-factor
uniform float3 		eye_position;
uniform float3		eye_direction;
uniform float3		eye_normal;
uniform	float4 		dt_params;
uniform float4		screen_res;
uniform float4		parallax;

uniform float4		hemi_cube_pos_faces;
uniform float4		hemi_cube_neg_faces;
uniform float4		L_material;				// 0,0,0,mid
uniform float4		Ldynamic_color;			// dynamic light color (rgb1)		- spot/point/sun
uniform float4		Ldynamic_pos;			// dynamic light pos+1/range(w)		- spot/point
uniform float4		Ldynamic_dir;			// dynamic light direction			- sun

uniform float4		J_direct[6];
uniform float4		J_spot[6];

// Глобальные параметры шейдеров --#SM+#--
uniform float4x4	m_script_params; 
uniform	float4		m_hud_params;	// zoom_rotate_factor, secondVP_zoom_factor, NULL, NULL
uniform	float4		m_blender_mode;	// x\y = [0 - default, 1 - night vision, 2 - thermal vision]; x - основной вьюпорт, y - второй впьюпорт, z = ?, w = [0 - идёт рендер обычного объекта, 1 - идёт рендер детальных объектов (трава, мусор)]

// Параметры, уникальные для разных моделей --#SM+#--
uniform float4x4	m_obj_camo_data; 
uniform float4		m_obj_custom_data;
uniform float4		m_obj_generic_data;

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
float3 	unpack_normal	(float3 v)	{ return 2*v-1;			}
float3 	unpack_bx2	(float3 v)	{ return 2*v-1; 		}
float3 	unpack_bx4	(float3 v)	{ return 4*v-2; 		} //!reduce the amount of stretching from 4*v-2 and increase precision

float2 	unpack_tc_base	(float2 tc, float du, float dv)		{
		return (tc.xy + float2	(du,dv))*(32.f/32768.f);	//!Increase from 32bit to 64bit floating point
}

float2 	unpack_tc_lmap	(float2 tc)	{ return tc*(1.f/32768.f);	} // [-1  .. +1 ] 

float 	calc_cyclic 	(float x)				{
	float 	phase 	= 1/(2*3.141592653589f);
	float 	sqrt2	= 1.4142136f;
	float 	sqrt2m2	= 2.8284271f;
	float 	f 	= sqrt2m2*frac(x)-sqrt2;	// [-sqrt2 .. +sqrt2] !No changes made, but this controls the grass wave (which is violent if I must say)
	return 	f*f - 1.f;				// [-1     .. +1]
}
float2 	calc_xz_wave 	(float2 dir2D, float frac)		{
	// Beizer
	float2  ctrl_A	= float2(0.f,		0.f	);
	float2 	ctrl_B	= float2(dir2D.x,	dir2D.y	);
	return  lerp	(ctrl_A, ctrl_B, frac);			//!This calculates tree wave. No changes made
}

#endif
