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

/*************************************************
* THIS CODE IS NOT A FINISHED.
* VERTVER: This code is does not deprecated in
* X-Ray Oxygen. Please, don't delete it. 
*************************************************/

#include "stdafx.h"

XCore::XCore()
{
	// load library if we have Windows 7
	//if (!IsWindows8OrGreater()) { XAudioDLL = LoadLibraryExA("XAudio2_7.DLL", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32); } R_ASSERT(XAudioDLL);
}

XCore::~XCore()
{
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
	if (xData.pXAudio)
	{
		xData.pXAudio->StopEngine();
	}
	_RELEASE(xData.pXAudio);

	xData.waveData.reset();

	//R_ASSERT(FreeLibrary(XAudioDLL));
}


LPCSTR GetUnicodeStringFromAnsi(LPCWSTR wString)
{
	std::wstring tempWstring(wString);
	std::string tempString(tempWstring.begin(), tempWstring.end());
	return tempString.c_str();
}

XSTATUS XCore::InitXAudioDevice()
{
	// create XAudio2 device
	if (FAILED(XAudio2Create(&xData.pXAudio))) { return XAUDIO_BAD_DEVICE; }

	// create master voice (needy for init 
	if (FAILED(xData.pXAudio->CreateMasteringVoice(&xData.pMasteringVoice))) { return XAUDIO_BAD_DEVICE; }

	// get device list
	lastStatus = GetDeviceList(xData.pXAudio, deviceList); 
	if (FAILEDX(lastStatus)) { return lastStatus; }

	return XAUDIO_OK;
}

XSTATUS XCore::GetDeviceList(IXAudio2* pXAudio, std::vector<XAUDIO_DEVICE>& deviceList)
{
	DWORD32 DeviceCount = 0;
	HRESULT hr = pXAudio->GetDeviceCount(&DeviceCount);

	// if our device is NULL or device count is NULL - return BAD_DEVICE
	if (SUCCEEDED(hr) || DeviceCount)
	{ 
		deviceList.reserve(DeviceCount);

		for (DWORD32 j = 0; j < DeviceCount; ++j)
		{
			// get to device details struct
			XAUDIO2_DEVICE_DETAILS details;
			hr = pXAudio->GetDeviceDetails(j, &details);
			if (SUCCEEDED(hr))
			{
				XAUDIO_DEVICE device = {};
				device.deviceId = GetUnicodeStringFromAnsi(details.DeviceID);
				device.deviceDescription = GetUnicodeStringFromAnsi(details.DisplayName);
				deviceList.emplace_back(device);
				Msg("Device ID: %s\nDevice name: %s", device.deviceId.c_str(), device.deviceDescription.c_str());
			}
		}
		return XAUDIO_OK;
	}
	else return XAUDIO_BAD_DEVICE;
}

XSTATUS XCore::SimpleAudioPlay(CSoundRender_Emitter* soundEmitter, CSoundRender_Source* soundSource)
{
	// copy wave data from WAVEFORMATEX;
	VERIFY(soundSource->m_wformat.nChannels);
	WAVEFORMATEX waveFormat = soundSource->m_wformat; 

	VERIFY(xData.pXAudio);
	if (FAILED(xData.pXAudio->CreateSourceVoice(&xData.pSourceVoice, &waveFormat))) { return XAUDIO_BAD_SAMPLE; }

	// create empty buffer
	XAUDIO2_BUFFER buffer = { NULL };	//#TODO: MUST BE POINTER HERE

	// submit this and play
	if (FAILED(xData.pSourceVoice->SubmitSourceBuffer(&buffer))) { return XAUDIO_BUFFER_CURREPTED; }
	if (FAILED(xData.pSourceVoice->Start(NULL))) { return XAUDIO_BUFFER_CURREPTED; }

	return XAUDIO_OK;
}

XSTATUS XCore::SetMasterVolume(float Volume)
{
	VERIFY(xData.pSourceVoice);

	VERIFY(Volume);
	soundGain.AudioGain = Volume;

	// set default sound effect 
	if (FAILED(xData.pSourceVoice->SetEffectParameters(1, &soundGain, sizeof(GAIN_LEVEL))))
	{
		return XAUDIO_BAD_SURROUND;
	}
	return XAUDIO_OK;
}
