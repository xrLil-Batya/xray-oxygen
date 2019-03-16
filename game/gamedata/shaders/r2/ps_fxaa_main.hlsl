#include "common.h"

#define FXAA_HLSL_3 1
#define FXAA_GREEN_AS_LUMA 0

#if !defined(PP_AA_QUALITY) || (PP_AA_QUALITY <= 1) || (PP_AA_QUALITY > 4)
	#define	FXAA_QUALITY__PRESET 10
#elif PP_AA_QUALITY == 2
	#define	FXAA_QUALITY__PRESET 20
#elif PP_AA_QUALITY == 3
	#define	FXAA_QUALITY__PRESET 29
#elif PP_AA_QUALITY == 4
	#define	FXAA_QUALITY__PRESET 39
#endif

// Choose the amount of sub-pixel aliasing removal. This can effect sharpness.
#define FXAA_QUALITY__SUBPIX 0.5f
//   1.00 - upper limit (softer)
//   0.75 - default amount of filtering
//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
//   0.25 - almost off
//   0.00 - completely off

// The minimum amount of local contrast required to apply algorithm.
#define FXAA_QUALITY__EDGE_THRESHOLD 0.063f
//   0.333 - too little (faster)
//   0.250 - low quality
//   0.166 - default
//   0.125 - high quality 
//   0.063 - overkill (slower)

// Trims the algorithm from processing darks.
#define FXAA_QUALITY__EDGE_THRESHOLD_MIN 0.0312f
//   0.0833 - upper limit (default, the start of visible unfiltered edges)
//   0.0625 - high quality (faster)
//   0.0312 - visible limit (slower)

#include "fxaa.h"

float4 main (p_screen I) : COLOR
{
	float4 img = FxaaPixelShader(
			I.tc0,									// FxaaFloat2 pos,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsolePosPos,
            s_image,								// FxaaTex tex,
            s_image,								// FxaaTex fxaaConsole360TexExpBiasNegOne,
            s_image,								// FxaaTex fxaaConsole360TexExpBiasNegTwo,
            screen_res.zw,							// FxaaFloat2 fxaaQualityRcpFrame,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
            FXAA_QUALITY__SUBPIX,					// FxaaFloat fxaaQualitySubpix,
            FXAA_QUALITY__EDGE_THRESHOLD,			// FxaaFloat fxaaQualityEdgeThreshold,
            FXAA_QUALITY__EDGE_THRESHOLD_MIN,		// FxaaFloat fxaaQualityEdgeThresholdMin,
            0.0f,									// FxaaFloat fxaaConsoleEdgeSharpness,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThreshold,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThresholdMin,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)		// FxaaFloat4 fxaaConsole360ConstDir,
            );
	img.w = 1.0f;
	
	return img;
}
