#include "common.h"
	
struct 	v2p
{
 	float2 	tc0: 		TEXCOORD0;	// base
 	float3 	tc1: 		TEXCOORD1;	// environment
  	float4	c0:			COLOR0;		// sun.(fog*fog)
};

//////////////////////////////////////////////////////////////////////////////////////////

// Pixel
uniform	float4		screen_res;

Texture2D 	s_vp2;
//Texture2D s_skymap;

//uniform samplerCUBE	s_env0;
//uniform samplerCUBE	s_env1;

float random(float2 co)
{
      return 0.5+(frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453))*0.5;
};

float4 main( v2p I ) : SV_Target
{
//	float2 	distort	= s_distort.Sample( smp_rtlinear, I.tc);
//	float3	image 	= s_base.Sample( smp_rtlinear, I.tc + offset);
	
	float4	t_base 		= s_base.Sample		( smp_base, I.tc0);		// �������� �����	
 //	float4	t_skymap 	= s_skymap.Sample	( smp_base, I.tc0); 	// ����� ��������� ����
	
	I.tc0.x = (I.tc0.x-0.5f)*(screen_res.y*screen_res.z)+0.5f;	// ����������� �������� � ����� ���, ����� �� ����� ���������� ������ ���� ���������� ���������	
	float4	t_vp2	 = s_vp2.Sample	( smp_base, I.tc0);  			// ����������� �� ������� ��������
	float3	final	 = float3(0, 0, 0);
	
	{	//** ����������� ����� **//
	
		// ������� � ��������� �������� ����
		//float3	env0		= texCUBE	(s_env0, I.tc1);
		//float3	env1		= texCUBE	(s_env1, I.tc1);
		//float3	env			= lerp		(env0, env1, L_ambient.w);	// �� ��������
		
		// ������� �� � �������� ��������� ��������
		float3 	base	= lerp	(t_vp2, t_base, t_base.a);		// ����� � ���������
				final	= base;
		//		final	= lerp	(base,  env,    t_skymap.a);	// base c �����
	}
	
	// out
	return  float4	(final.r, final.g, final.b, m_hud_params.x);
}
