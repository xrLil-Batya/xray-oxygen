/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: XAudio Core
**********************************************************
* XAudioCore.cpp
* Main methods for XAudio implementation
*********************************************************/
#include "stdafx.h"
#pragma comment(lib,"x3daudio.lib")

XRSOUND_API XCore coreAudio;

XCore::XCore() 
{ 
	// init COM-pointers if not editor 
	// because xrEditor written on C# + C++/CLI and use COM-pointers
	//if (!strstr(Core.Params, "-editor")) { CoInitializeEx(NULL, COINIT_MULTITHREADED); }

	// load any version of XAudio2
	XAudioDLL = ::LoadLibraryExA("XAudio2_9.DLL", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!XAudioDLL)
	{
		XAudioDLL = ::LoadLibraryExA("XAudio2_8.DLL", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (!XAudioDLL)
		{
			XAudioDLL = ::LoadLibraryExA("XAudio2_7.DLL", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		}
	}

	R_ASSERT(XAudioDLL);
}

XCore::~XCore()
{
	// destroy all stuff
	if (xData.pSourceVoice)
	{
		xData.pSourceVoice->DestroyVoice();
		xData.pSourceVoice = nullptr;
	}
	if (xData.pMasteringVoice)
	{
		xData.pMasteringVoice->DestroyVoice();
		xData.pMasteringVoice = nullptr;
	}
	if (xData.pSubmixVoice)
	{
		xData.pSubmixVoice->DestroyVoice();
		xData.pSubmixVoice = nullptr;
	}

	// we must stop engine before release 
	// can include buffered samples in mem
	if (xData.pXAudio) { xData.pXAudio->StopEngine();}
	_RELEASE(xData.pXAudio);

	// free library and unitialize COM-pointers
	if (XAudioDLL) { FreeLibrary(XAudioDLL); }
}

LPCSTR GetAnsiStringFromUnicodeString(LPCWSTR wString)
{
	size_t outputSize = wcslen(wString) + 1;
	LPSTR newString = (LPSTR)Memory.mem_alloc(outputSize);
	size_t charsConverted = 0;

	wcstombs_s(&charsConverted, newString, outputSize, wString, strlen(newString));
	R_ASSERT(charsConverted == outputSize);

	return newString;
}

XSTATUS XCore::InitXAudioDevice()
{
	// create XAudio2 device
	if (FAILED(XAudio2Create(&xData.pXAudio))) { return XAUDIO_BAD_DEVICE; }

	// get device list
	lastStatus = GetDeviceList(xData.pXAudio, deviceList);
	if (FAILEDX(lastStatus)) { return lastStatus; }

	// search for default device
	DWORD32 devCount = 0;
	INT32 devIndex = -1;
	for (auto it = deviceList.cbegin(); it != deviceList.cend(); ++it, ++devCount)
	{
		// simple selection criteria of just picking the first one
		if (devIndex == -1) { devIndex = devCount; }
	}

	// create master voice (needy for init)
	if (FAILED(xData.pXAudio->CreateMasteringVoice(&xData.pMasteringVoice))) { return XAUDIO_BAD_DEVICE; }

	// get device info for create submix voice
	DWORD ChannelMask = NULL;
	xData.deviceDetail = { NULL };
	
	// get device info to struct
	lastStatus = GetDeviceInfo(deviceList[devIndex], &xData.deviceDetail);
	if (FAILEDX(lastStatus)) { return lastStatus; }
	ChannelMask = xData.deviceDetail.OutputFormat.dwChannelMask;

	// create reverb effect
	if (FAILED(XAudio2CreateReverb(&xData.pReverb))) { return XAUDIO_BAD_SURROUND; }

	// get reverb effect descriptors 
	// and chain to manipulate with it
	XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { xData.pReverb, TRUE, 1 } };
	XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

	// create submix voice with reverb
	if (FAILED(xData.pXAudio->CreateSubmixVoice(
		&xData.pSubmixVoice, 1,
		xData.deviceDetail.OutputFormat.Format.nSamplesPerSec,
		0, 0, nullptr, &effectChain)))
	{
		return XAUDIO_BAD_DEVICE;
	}

	// create X3DAudio interface
	const float SpeedOfSound = 343.33;		// (343 1/3 m/s) 20 degreeses with cloudy weather
	X3DAudioInitialize(ChannelMask, SpeedOfSound, xData.x3DInstance);

	return XAUDIO_OK;
}

XSTATUS XCore::GetDeviceList(IXAudio2* pXAudio, xr_vector<XAUDIO_DEVICE>& refDeviceList)
{
	DWORD32 DeviceCount = 0;
	HRESULT hr = NULL;

	R_ASSERT(pXAudio);
	hr = pXAudio->GetDeviceCount(&DeviceCount);

	// if our device is NULL or device count is NULL - return BAD_DEVICE
	if (SUCCEEDED(hr) || DeviceCount)
	{
		refDeviceList.reserve(DeviceCount);

		for (DWORD32 j = 0; j < DeviceCount; ++j)
		{
			// get to device details struct
			XAUDIO2_DEVICE_DETAILS details = { NULL };
			hr = pXAudio->GetDeviceDetails(j, &details);
			if (SUCCEEDED(hr))
			{
				XAUDIO_DEVICE device = {};
				device.deviceId = GetAnsiStringFromUnicodeString(details.DeviceID);
				device.deviceDescription = GetAnsiStringFromUnicodeString(details.DisplayName);
				refDeviceList.emplace_back(device);
				Msg("Device ID: %s\nDevice name: %s", device.deviceId.c_str(), device.deviceDescription.c_str());
			}
		}
		return XAUDIO_OK;
	}
	else return XAUDIO_BAD_DEVICE;
}

XSTATUS XCore::GetDeviceInfo(XAUDIO_DEVICE DeviceInfo, XAUDIO2_DEVICE_DETAILS* DeviceDetails)
{
	VERIFY(xData.pXAudio);
	if (FAILED(xData.pXAudio->GetDeviceDetails(std::stoi(DeviceInfo.deviceId.c_str()), DeviceDetails)))
	{
		return XAUDIO_BAD_DEVICE;
	}

	Msg("Device ID: %s ", GetAnsiStringFromUnicodeString(DeviceDetails->DeviceID));
	Msg("Device Name: %s ", GetAnsiStringFromUnicodeString(DeviceDetails->DisplayName));
	Msg("Device Format: %i ", DeviceDetails->OutputFormat.Format.wFormatTag);		// if 1 - wave 
	Msg("Device Channels: %i ", DeviceDetails->OutputFormat.Format.nChannels);
	Msg("Device Rate: %i ", DeviceDetails->OutputFormat.Format.nSamplesPerSec);
	Msg("Device Bitrate: %i ", DeviceDetails->OutputFormat.Format.wBitsPerSample);
	switch (DeviceDetails->Role)
	{
	case NotDefaultDevice:
		Msg("Device Role: Not default device");
		break;
	case DefaultConsoleDevice:
		Msg("Device Role: default console device");
		break;
	case DefaultMultimediaDevice:
		Msg("Device Role: default multimedia device");
		break;
	case DefaultCommunicationsDevice:
		Msg("Device Role: default communication device");
		break;
	case DefaultGameDevice:
		Msg("Device Role: default game device");
		break;
	case GlobalDefaultDevice:
		Msg("Device Role: default global device");
		break;
	case InvalidDeviceRole:
	default:
		Msg("Device Role: invalid role device");
		break;
	}

	return XAUDIO_OK;
}

// for test (don't use)
XSTATUS XCore::SimpleAudioPlay(CSoundRender_Emitter* soundEmitter, CSoundRender_Source* soundSource)
{
	HANDLE hFile = NULL;
	DWORD dwFileSize = NULL;
	DWORD dwSizeWritten = NULL;

	hFile = CreateFileA("I:\\glint_preview.wav", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	R_ASSERT(hFile);
	dwFileSize = GetFileSize(hFile, NULL);
	LPVOID lpData =  Memory.mem_alloc(dwFileSize);
	R_ASSERT(ReadFile(hFile, lpData, dwFileSize, &dwSizeWritten, NULL));

	// copy wave data from WAVEFORMATEX;
	VERIFY(soundSource->m_wformat.nChannels);
	WAVEFORMATEX waveFormat = soundSource->m_wformat;

	VERIFY(xData.pXAudio);
	if (FAILED(xData.pXAudio->CreateSourceVoice(&xData.pSourceVoice, &waveFormat))) { return XAUDIO_BAD_SAMPLE; }

	// create empty buffer
	XAUDIO2_BUFFER buffer = { NULL };	
	buffer.AudioBytes = dwSizeWritten;
	buffer.pAudioData = (BYTE*)lpData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	// submit this and play
	if (FAILED(xData.pSourceVoice->SubmitSourceBuffer(&buffer))) { return XAUDIO_BUFFER_CURREPTED; }
	if (FAILED(xData.pSourceVoice->Start(NULL))) { return XAUDIO_BUFFER_CURREPTED; }

	return XAUDIO_OK;
}

XSTATUS XCore::SetMasterVolume(float Volume)
{
	VERIFY(xData.pSourceVoice);

	soundGain.AudioGain = Volume;

	// set default sound effect 
	if (FAILED(xData.pMasteringVoice->SetEffectParameters(1, &soundGain, sizeof(GAIN_LEVEL))))
	{
		return XAUDIO_BAD_SURROUND;
	}
	return XAUDIO_OK;
}

void CSoundRender_CoreB::_restart()
{
	inherited::_restart();
}

void CSoundRender_CoreB::update_listener(const Fvector& P, const Fvector& D, const Fvector& N, float dt)
{
	// D Vector - first listener orientation vector
	// N Vector - second listener orientation vector
	// P Vector - listener position vector
	// dt - must be 0.0f
	inherited::update_listener(P, D, N, dt);

	if (!Listener.position.similar(P))
	{
		Listener.position.set(P);
		bListenerMoved = true;
	}
	Listener.orientation[0].set(D.x, D.y, -D.z);
	Listener.orientation[1].set(N.x, N.y, -N.z);
}

void CSoundRender_CoreB::_initialize(int stage)
{
	// init XAudio2 pointers
	if (FAILEDX(coreAudio.InitXAudioDevice()))
	{
		R_ASSERT2(false, "xAudio2 devices not found!");
		return;
	}

	{
		//#TODO: Source voice wave format
		WAVEFORMATEX waveFormat = { NULL };
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = 2;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = waveFormat.wBitsPerSample / 8 * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	}

	// get data to listener 
	audioState.listener.OrientFront.x = 0.0f;
	audioState.listener.OrientFront.y = 0.0f;
	audioState.listener.OrientFront.z = 1.0f;
	audioState.listener.OrientTop.x = 0.0f;
	audioState.listener.OrientTop.y = 1.0f;
	audioState.listener.OrientTop.z = 0.0f;
	audioState.listener.pCone = nullptr;
	audioState.listener.Position.x = 0.0f;
	audioState.listener.Position.y = 0.0f;
	audioState.listener.Position.z = 0.0f;
	audioState.listener.Velocity.x = 0.0f;
	audioState.listener.Velocity.y = 0.0f;
	audioState.listener.Velocity.z = 0.0f;

	inherited::_initialize(stage);

	//first initialize
	if (stage == 1)
	{
		// Pre-create targets
		CSoundRender_Target* T = 0;
		for (u32 tit = 0; tit < u32(psSoundTargets); ++tit)
		{
			T = new CSoundRender_TargetB();
			if (T->_initialize())
			{
				s_targets.push_back(T);
			}
			else
			{
				Msg("[xAudio2] ! SOUND: xAudio2: Max targets - %u", tit);
				T->_destroy();
				xr_delete(T);
				break;
			}
		}
	}
	
	Msg("XAudio2 was initialized on device: %s", GetAnsiStringFromUnicodeString(coreAudio.xData.deviceDetail.DisplayName));
}

void CSoundRender_CoreB::_clear()
{
	inherited::_clear();
	
	// remove all targets
	CSoundRender_Target* T = 0;
	for (u32 tit = 0; tit < s_targets.size(); tit++)
	{
		T = s_targets[tit];
		T->_destroy();
		xr_delete(T);
	}
}

void CSoundRender_CoreB::set_master_volume(float f)
{
	float audioVolume = f;									
	clamp(audioVolume, EPS_S, 1.f);

	// convert to decibels
	float HWVolume = fFloorSSE2(7000.f * logf(audioVolume) / 5.f);
	clamp(HWVolume, -10000.f, 0.f);

	coreAudio.SetMasterVolume(XAudio2DecibelsToAmplitudeRatio(HWVolume));
}

void AudioCallback::OnBufferEnd(void* BufferContext)
{
	VERIFY(BufferContext); 
}
