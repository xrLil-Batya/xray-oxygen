////////////////////////////////////
// ForserX to X-Ray Oxygen Project
#include "common.h"

#define DX9Ver
#define PIXEL_SIZE screen_res.zw
#include "toolbox\dlaa.h"

float4 main(p_screen I) : COLOR
{
	return DLAAPixelShader(I.tc0);
}