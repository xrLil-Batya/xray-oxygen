#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

// Reflection vector
float3 calc_reflection(float3 pos_w, float3 norm_w)
{
    return reflect(normalize(pos_w - eye_position), norm_w);
}

// Environment map
float3 calc_envmap(float3 vreflect)
{
	float3	env0 = texCUBElod (s_env0, float4(vreflect.xyz, 0.0f)).xyz;
	float3	env1 = texCUBElod (s_env1, float4(vreflect.xyz, 0.0f)).xyz;
	return lerp(env0, env1, L_ambient.w);
}

// Fog
float calc_fogging(float4 w_pos)
{
	return dot(w_pos, fog_plane);
}

// Detail texture
float2 calc_detail(float3 w_pos)
{
	float	dtl		= distance(w_pos,eye_position)*dt_params.w;
			dtl		= min(dtl*dtl, 1);
	float	dt_mul	= 1.0f  - dtl;	// dt*  [1 ..   0]
	float	dt_add	= 0.5f * dtl;	// dt+  [0 .. 0.5]
	return float2(dt_mul,dt_add);
}

// R1-style lighting
float3 calc_sun_r1                (float3 norm_w)    { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));                 }
float3 calc_model_hemi_r1         (float3 norm_w)    { return max(0,norm_w.y)*L_hemi_color.xyz;                                         }
float3 calc_model_lq_lighting     (float3 norm_w)    { return L_material.xxx*calc_model_hemi_r1(norm_w) + L_ambient.xyz + L_material.y*calc_sun_r1(norm_w);         }

// Tonemapping
void tonemap(out float4 low, out float4 high, float3 rgb, float scale)
{
	rgb *= scale;

	const float fWhiteIntensity = 1.7f;
	const float fWhiteIntensitySQR = fWhiteIntensity*fWhiteIntensity;
#ifdef USE_BRANCHING // ps_3_0
	low		=	((rgb*(1.0f + rgb/fWhiteIntensitySQR)) / (rgb + 1.0f)).xyzz;
	high	=	low/def_hdr;        // 8x dynamic range
#else
	low		=	float4(((rgb*(1.0f + rgb/fWhiteIntensitySQR)) / (rgb+1.0f)), 0);
	high	=	float4(rgb/def_hdr, 0.0f);		// 8x dynamic range
#endif
}

// Bloom combine
float4 combine_bloom(float3 low, float4 high)
{
	return float4(low + high.xyz*high.a, 1.0f);
}


float3	v_hemi        	(float3 n)                        	{		return L_hemi_color.xyz*(0.5f + 0.5f*n.y);				}
float3	v_hemi_wrap     (float3 n, float w)                	{		return L_hemi_color.xyz*(w + (1.0f - w)*n.y);			}
float3	v_sun           (float3 n)                        	{		return L_sun_color*dot(n,-L_sun_dir_w);					}
float3	v_sun_wrap      (float3 n, float w)                	{		return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));		}
float3	p_hemi          (float2 tc)							{		return tex2D(s_hemi, tc).a;								}

float get_hemi(float4 lmh)
{
	return lmh.a;
}

float get_sun(float4 lmh)
{
	return lmh.g;
}

//	contrast function
float Contrast(float Input, float ContrastPower)
{
     //piecewise contrast function
     bool IsAboveHalf = Input > 0.5 ;
     float ToRaise = saturate(2*(IsAboveHalf ? 1-Input : Input));
     float Output = 0.5*pow(ToRaise, ContrastPower);
     Output = IsAboveHalf ? 1-Output : Output;
     return Output;
}

// Convert projected coords to screen coords
float4 proj2screen(float4 Project)
{
	float4	Screen		= Project;
			Screen.x	= (Project.x + Project.w);
			Screen.y	= (Project.w - Project.y);
			Screen.xy  *= 0.5f;
			
	return Screen;
}

// Check if given depth value belongs to sky or not
float is_sky(float depth)
{
	return step(abs(depth - SKY_DEPTH), SKY_EPS);
}

float is_not_sky(float depth)
{
	return step(SKY_EPS, abs(depth - SKY_DEPTH));
}

#endif
