/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: XAudio Core
**********************************************************
* XAudioCore.h
* Main methods for XAudio implementation
*********************************************************/
#pragma once

#include "../xrCore/xrCore.h"
#include "SoundRender_Source.h"
#include "SoundRender_Emitter.h"

#include <windows.h>
#include <Unknwnbase.h>
#include <stdint.h>
#include <objbase.h>
#include <memory>
#include <mmreg.h>
#include <stdio.h>
#include "dx/XAudio2.h"
#include "dx/XAudio2fx.h"
#include "dx/X3DAudio.h"
#include <dx/comdecl.h>
#include <ppltasks.h>

enum XSTATUS
{
	XAUDIO_OK = 0,
	XAUDIO_BAD_DEVICE,
	XAUDIO_BAD_SAMPLE,
	XAUDIO_BUFFER_CURREPTED,
	XAUDIO_BAD_SURROUND
};

#define FAILEDX(x) (!(x == XAUDIO_OK))
#define SUCCEEDEDX(x) (x == XAUDIO_OK)
#define _RELEASE(x)			{ if(x) { (x)->Release(); (x)=NULL; } }

#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample

#define NUM_PRESETS 30

XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[NUM_PRESETS] =
{
	XAUDIO2FX_I3DL2_PRESET_FOREST,			XAUDIO2FX_I3DL2_PRESET_DEFAULT,			XAUDIO2FX_I3DL2_PRESET_GENERIC,
	XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,		XAUDIO2FX_I3DL2_PRESET_ROOM,			XAUDIO2FX_I3DL2_PRESET_BATHROOM,
	XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,		XAUDIO2FX_I3DL2_PRESET_STONEROOM,		XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
	XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,		XAUDIO2FX_I3DL2_PRESET_CAVE,			XAUDIO2FX_I3DL2_PRESET_ARENA,
	XAUDIO2FX_I3DL2_PRESET_HANGAR,			XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY, XAUDIO2FX_I3DL2_PRESET_HALLWAY,
	XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,	XAUDIO2FX_I3DL2_PRESET_ALLEY,			XAUDIO2FX_I3DL2_PRESET_CITY,
	XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,		XAUDIO2FX_I3DL2_PRESET_QUARRY,			XAUDIO2FX_I3DL2_PRESET_PLAIN,
	XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,		XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,		XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
	XAUDIO2FX_I3DL2_PRESET_SMALLROOM,		XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,		XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
	XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,		XAUDIO2FX_I3DL2_PRESET_LARGEHALL,		XAUDIO2FX_I3DL2_PRESET_PLATE
};

using XAUDIO_DATA = struct
{
	IXAudio2*					pXAudio;
	IXAudio2MasteringVoice*		pMasteringVoice;
	IXAudio2SourceVoice*		pSourceVoice;
	IXAudio2SubmixVoice*		pSubmixVoice;
	IXAudio2VoiceCallback*		pCallback;

	XAUDIO2_VOICE_STATE			voiceState;
	XAUDIO2_VOICE_SENDS			voiceSends;
	std::unique_ptr<uint8_t[]>	waveData;

};

using XAUDIO_DEVICE = struct
{
	xr_string	deviceId;
	xr_string	deviceDescription;
};

struct GAIN_LEVEL
{
	float AudioGain;
};

class XRSOUND_API XCore
{
public:
	XCore();
	~XCore();

	XSTATUS InitXAudioDevice();
	XSTATUS GetDeviceList(IXAudio2* pXAudio, std::vector<XAUDIO_DEVICE>& deviceList);

	XSTATUS SimpleAudioPlay(CSoundRender_Emitter* soundEmitter, CSoundRender_Source* soundSource);
	XSTATUS SetMasterVolume(float Volume);

	std::vector<XAUDIO_DEVICE> deviceList;
private:
	XSTATUS		lastStatus;
	HMODULE		XAudioDLL;
	GAIN_LEVEL	soundGain;
	XAUDIO_DATA xData;

};
class XRSOUND_API XSurround
{

};