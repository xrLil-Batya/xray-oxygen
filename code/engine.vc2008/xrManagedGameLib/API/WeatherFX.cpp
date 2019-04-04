#include "stdafx.h"
#include "WeatherFX.h"
#include "xrGame\Level.h"

using namespace System;
using XRay::WeatherFX;

bool WeatherFX::Enable::get()
{
	return ::Environment().IsWeatherFXPlaying();
}

void WeatherFX::Create(::System::String^ str)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().SetWeatherFX(WetNameStr);
	}
}

void WeatherFX::Stop()
{
	(::Environment().StopWeatherFX());
}