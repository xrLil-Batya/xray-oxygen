#include "common.h"

float4 	main(p_screen I )	: COLOR
{
  float3 col;
  float factor = saturate(distance(I.tc0, float2(0.5, 0.5)));
         col.r =tex2D(s_image, float2(I.tc0 + float2(screen_res.z * factor, 0))).r;
         col.g =tex2D(s_image, float2(I.tc0 + float2(-0.866, -0.5) *screen_res.zw *factor)).g;
         col.b =tex2D(s_image, float2(I.tc0 + float2(0.866, -0.5) *screen_res.zw *factor)).b;
  //       col *= 1.4 - (1.4 *saturate(distance(I.tc0, float2(0.5f, 0.5f))));

  return float4(col,1);
}
