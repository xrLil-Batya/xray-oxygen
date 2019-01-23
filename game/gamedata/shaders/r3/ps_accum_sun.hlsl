#include "common.h"

#ifdef MSAA_OPTIMIZATION
#define MSAA_OPT_ARGS , uint iSample : SV_SAMPLEINDEX
#else
#define MSAA_OPT_ARGS
#endif

float4 main(p_aa_AA_sun I, float4 pos2d : SV_Position MSAA_OPT_ARGS) : SV_Target
{
	return float4(1.0,1.0,1.0,1.0);
}
