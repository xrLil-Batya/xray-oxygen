// By qweasdd
#include "common.h"

uniform float4 screen_res;		// Screen resolution (x-Width,y-Height, zw - 1/resolution)

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_screen main(p_screen I)
{
 	v2p_screen O;
	
	{
		I.hpos.xy += 0.5f;
		O.HPos.x = I.hpos.x * screen_res.z * 2 - 1;
		O.HPos.y = (I.hpos.y * screen_res.w * 2 - 1)*-1;
		O.HPos.zw = I.hpos.zw;
	}
    O.tc0 = I.tc0;
  
    return O; 
}