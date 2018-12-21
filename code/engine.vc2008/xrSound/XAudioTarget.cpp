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

extern XRSOUND_API XCore coreAudio;

CSoundRender_TargetB::CSoundRender_TargetB() : CSoundRender_Target()
{
	psSpeedOfSound = 1.f;
	pSource = nullptr;
}

CSoundRender_TargetB::~CSoundRender_TargetB()
{
}

bool CSoundRender_TargetB::_initialize()
{
	inherited::_initialize();

	return false;
}

void CSoundRender_TargetB::_destroy()
{
	xr_delete(pSource);
//	inherited::_destroy();
}

void CSoundRender_TargetB::_restart()
{
	_destroy();
	_initialize();
}

void CSoundRender_TargetB::fill_block(u32 BufferID)
{
	
}

void CSoundRender_TargetB::start(CSoundRender_Emitter * E)
{
	inherited::start(E);
}

void CSoundRender_TargetB::render()
{
	for (u32 buf_idx = 0; buf_idx < sdef_target_count; buf_idx++)
		fill_block(*(u32*)pBuffers[buf_idx]); //-V206

	inherited::render();
}

void CSoundRender_TargetB::rewind()
{
	inherited::rewind();
}

void CSoundRender_TargetB::stop()
{
	inherited::stop();
}

void CSoundRender_TargetB::update()
{
	inherited::update();
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

void CSoundRender_TargetB::alAuxInit(u32 slot)
{
}
