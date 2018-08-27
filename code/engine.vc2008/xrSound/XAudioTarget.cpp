/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: XAudio Target
**********************************************************
* XAudioTarget.cpp
* Main methods for XAudio implementation
*********************************************************/

#include "stdafx.h"

void CSoundRender_TargetB::fill_block(u32 BufferID)
{

}

void CSoundRender_TargetB::fill_parameters()
{
	CSoundRender_Emitter* SE = m_pEmitter; VERIFY(SE);

	inherited::fill_parameters();

	// 3D params
	VERIFY2(m_pEmitter, SE->source()->file_name());

	//XAUDIO2FX_REVERB_PARAMETERS params;
	//ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS[0], &params);
	//R_CHK(xData.pSubmixVoice->SetEffectParameters(NULL, &params, sizeof(XAUDIO2FX_REVERB_PARAMETERS)));
}