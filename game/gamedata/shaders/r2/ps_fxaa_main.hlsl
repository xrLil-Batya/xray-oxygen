#include "common.h"
#include "fxaa.h"

struct v2p 
{
	half4 	HPos : POSITION;
	half2 	tc0	 : TEXCOORD0;
};

half4 main (v2p I) : COLOR
{
	half4 img = FxaaPixelShader(
			I.tc0,									// FxaaFloat2 pos,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsolePosPos,
            s_image,								// FxaaTex tex,
            s_image,								// FxaaTex fxaaConsole360TexExpBiasNegOne,
            s_image,								// FxaaTex fxaaConsole360TexExpBiasNegTwo,
            screen_res.zw,							// FxaaFloat2 fxaaQualityRcpFrame,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
            0.35f,									// FxaaFloat fxaaQualitySubpix,
            0.125f,									// FxaaFloat fxaaQualityEdgeThreshold,
            0.0f,//0.0625f,							// FxaaFloat fxaaQualityEdgeThresholdMin,
            0.0f,									// FxaaFloat fxaaConsoleEdgeSharpness,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThreshold,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThresholdMin,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)		// FxaaFloat4 fxaaConsole360ConstDir,
            );
	
	img.w = 1.0f;
	
	return saturate(img);
}
