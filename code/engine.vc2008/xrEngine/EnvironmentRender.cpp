// EnvironmentRender.cpp
// Interaction of CEnvironment class with renderers

#include "stdafx.h"
#pragma hdrstop

#include "Environment.h"
#include "IGame_Level.h"
#ifndef _EDITOR
    #include "Render.h"
#endif
// Effects
#include "LensFlare.h"
#include "Rain.h"
#include "Thunderbolt.h"

void CEnvironment::RenderSky()
{
#ifndef _EDITOR
	if (!g_pGameLevel)
		return;
#endif
	m_pRender->RenderSky(*this);
}

void CEnvironment::RenderClouds()
{
#ifndef _EDITOR
	if (!g_pGameLevel)
		return;
#endif
	// draw clouds
	if (fis_zero(CurrentEnv->clouds_color.w, EPS_L))
		return;

	m_pRender->RenderClouds(*this);
}

void CEnvironment::RenderFlares()
{
#ifndef _EDITOR
	if (!g_pGameLevel)
		return;
#endif
	// 1
	eff_LensFlare->Render(false, true, true);
}

void CEnvironment::RenderLast()
{
#ifndef _EDITOR
	if (!g_pGameLevel)
		return;
#endif
	// 2
	eff_Rain->Render		();
	eff_Thunderbolt->Render	();
}

void CEnvironment::OnDeviceCreate()
{
	m_pRender->OnDeviceCreate();

	// Weathers
	for (auto &wCycle : WeatherCycles)
	{
		for (CEnvDescriptor* envDescriptor : wCycle.second)
			envDescriptor->OnDeviceCreate();
	}

	// Effects
	for (auto &wFX : WeatherFXs)
	{
		for (CEnvDescriptor* envDescriptor : wFX.second)
			envDescriptor->OnDeviceCreate();
	}

	Invalidate	();
	OnFrame		();
}

void CEnvironment::OnDeviceDestroy()
{
	m_pRender->OnDeviceDestroy();

	// Weathers
	for (auto &wCycle : WeatherCycles)
	{
		for (CEnvDescriptor* envDescriptor : wCycle.second)
			envDescriptor->OnDeviceDestroy();
	}

	// Effects
	for (auto &wFX : WeatherFXs)
	{
		for (CEnvDescriptor* envDescriptor : wFX.second)
			envDescriptor->OnDeviceDestroy();
	}

	CurrentEnv->Destroy();
}
