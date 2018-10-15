#include "common.h"

#ifndef ISAMPLE
#define ISAMPLE 0
#endif

#ifndef MSAA_OPTIMIZATION
float4 main(p_screen I) : SV_Target
#else
float4 main(p_screen I, uint iSample : SV_SAMPLEINDEX) : SV_Target
#endif
{
#ifndef USE_MSAA
	return		s_generic.Load(int3(I.hpos.xy, 0), 0);
#else
#ifndef MSAA_OPTIMIZATION
	return		s_generic.Load(int3(I.hpos.xy, 0), ISAMPLE);
#else
	return		s_generic.Load(int3(I.hpos.xy, 0), iSample);
#endif
#endif
}
