#include "common.h"

struct 	v2p
{
 	float2 	tc0: 		TEXCOORD0;	// base
 	half3 	tc1: 		TEXCOORD1;	// environment
  	half4	c0:			COLOR0;		// sun.(fog*fog)
};

//////////////////////////////////////////////////////////////////////////////////////////

float  resize(float input, float factor, float offset)
{
	return (input-0.5f+offset) / factor+0.5f-offset;
}

// Pixel
uniform	float4		screen_res;

uniform sampler2D	s_vp2;
uniform sampler2D	s_skymap;

uniform samplerCUBE	s_env0;
uniform samplerCUBE	s_env1;

float random(float2 co)
{
      return 0.5+(frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453))*0.5;
};

half4 	main	( v2p I )	: COLOR
{
	half4	t_base 		= tex2D		(s_base,	I.tc0);		// �������� �����
	half4	t_skymap 	= tex2D		(s_skymap,	I.tc0);		// ����� ��������� ����
	
	// ����������� �������� � ����� ���, ����� �� ����� ���������� ������ ���� ���������� ���������
	I.tc0.x = resize(I.tc0.x, screen_res.x/screen_res.y, 0);	
	half4	t_vp2	 = tex2D		(s_vp2, I.tc0);			// ����������� �� ������� ��������
	half3	final	 = half3(0, 0, 0);
	
	if (m_blender_mode.x == 1.f)
	{	//** ������ ����� **//
	
		// ������������ ��������� ��� �������
		float noise		= random(I.tc0*timers.z) * 0.3;
		
		// ��������� �������� � 7 ���
		t_vp2.rgb *= 7;
		
		// ������������� �������
		float gray = ((t_vp2.r + t_vp2.g + t_vp2.b)/3);
		t_vp2.rgb = half3(gray, gray, gray);
		
		// ��������� ������ �������
		t_vp2.g += (0.4 + noise);
		
		// ������� � ������
		final	= lerp	(t_vp2, t_base, t_base.a);
	}
	else
	{	//** ����������� ����� **//
	
		// ������� � ��������� �������� ����
		half3	env0		= texCUBE	(s_env0, I.tc1);
		half3	env1		= texCUBE	(s_env1, I.tc1);
		half3	env			= lerp		(env0, env1, L_ambient.w);	// �� ��������
		
		// ������� �� � �������� ��������� ��������
		half3 	base	= lerp	(t_vp2, t_base, t_base.a);		// ����� � ���������
		//		final	= lerp	(base,  env,    t_skymap.a);	// base c �����
		final	= base;
	}
	
	// out
	return  half4	(final.r, final.g, final.b, m_hud_params.x);
}
