///////////////////////////////////////////////////
// http://theorangeduck.com/page/pure-depth-ssao
///////////////////////////////////////////////////
// Oxygen Engine 2016-2019
///////////////////////////////////////////////////

#include "common.h"
uniform float3x4 m_v2w;

float3 normal_from_depth(float depth, float2 texcoords) 
{
  const float2 offset1 = float2(0.f,0.001f);
  const float2 offset2 = float2(0.001f,0.f);
  
  float depth1 = tex2D(s_position, texcoords + offset1).r;
  float depth2 = tex2D(s_position, texcoords + offset2).r;
  
  float3 p1 = float3(offset1, depth1 - depth);
  float3 p2 = float3(offset2, depth2 - depth);
  
  float3 normal = cross(p1, p2);
  normal.z = -normal.z;
  
  return normalize(normal);
}

float4 main(p_screen ScreenRes): SV_Target
{
  const float total_strength = 1.2f;
  const float base = 0.35f; 
  const float area = 0.0075f;
  const float falloff = 0.000001f;
  const float radius = 0.00045f;
  
  const int samples = 16;
  float3 sample_sphere[samples] = 
  {
      float3( 0.5381f,  0.1856f, -0.4319f), float3( 0.1379f, 0.2486f, 0.4430f),
      float3( 0.3371f,  0.5679f, -0.0057f), float3(-0.6999f,-0.0451f,-0.0019f),
      float3( 0.0689f, -0.1598f, -0.8547f), float3( 0.0560f, 0.0069f,-0.1843f),
      float3(-0.0146f,  0.1402f,  0.0762f), float3( 0.0100f,-0.1924f,-0.0344f),
      float3(-0.3577f, -0.5301f, -0.4358f), float3(-0.3169f, 0.1063f, 0.0158f),
      float3( 0.0103f, -0.5869f,  0.0046f), float3(-0.0897f,-0.4940f, 0.3287f),
      float3( 0.7119f, -0.0154f, -0.0918f), float3(-0.0533f, 0.0596f,-0.5411f),
      float3( 0.0352f, -0.0631f,  0.5460f), float3(-0.4776f, 0.2847f,-0.0271f)
  };
  
  float3 random = normalize(s_jitter_5.load(int3(ScreenRes.tc0 * 4.0, 0).rgb);
  
  float depth = s_position.load(int3(ScreenRes.tc0), 0), 0).r;
 
  float3 position = float3(ScreenRes.tc0, depth);
  float3 normal = normal_from_depth(depth, ScreenRes.tc0);
  
  float radius_depth = radius / depth;
  float occlusion = 0.f;
  for(int i=0; i < samples; i++) 
  {
    float3 ray = radius_depth * reflect(sample_sphere[i], random);
    float3 hemi_ray = position + sign(dot(ray,normal)) * ray;
    
    float occ_depth = s_position.load(int3(saturate(hemi_ray.xy), 0), 0).r;
    float difference = depth - occ_depth;
    
    occlusion += step(falloff, difference) * (1.0f - smoothstep(falloff, area, difference));
  }
  
  float ao = 1.0f - total_strength * occlusion * (1.0f / samples);
  float occ = saturate(ao + base);
  return float4(occ, occ, occ, 1.0f); 
}
