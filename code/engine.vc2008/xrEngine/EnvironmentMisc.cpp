// EnvironmentMisc.cpp
// Interaction of CEnvironment class with other environment subclasses and effects

#include "stdafx.h"
#include <ppl.h>
#pragma hdrstop

#include "Environment.h"
#include "LensFlare.h"
#include "Thunderbolt.h"
#include "ThunderboltDesc.h"
#include "ThunderboltCollection.h"
#include "Rain.h"

#include "IGame_Level.h"
#include "../xrServerEntities/object_broker.h"

//-----------------------------------------------------------------------------
// Environment IO
//-----------------------------------------------------------------------------
CEnvAmbient* CEnvironment::AppendEnvAmb(const shared_str& sect)
{
	for (CEnvAmbient* &envAmbient : Ambients)
	{
		if (envAmbient->name() == sect)
			return envAmbient;
	}

	Ambients.emplace_back(xr_new<CEnvAmbient>());
	Ambients.back()->Load(*m_ambients_config, *m_sound_channels_config, *m_effects_config, sect);
	return Ambients.back();
}

void CEnvironment::ModsLoad()
{
	Modifiers.clear();

	string_path path;
	if (FS.exist(path, "$level$", "level.env_mod"))
	{
		IReader* fs = FS.r_open(path);
		u32 id = 0;
		u32 ver = 0x0015;
		u32 sz;

		while (0 != (sz = (u32)fs->find_chunk(id)))
		{
			if (id == 0 && sz == sizeof(u32))
			{
				ver = fs->r_u32();
			}
			else
			{
				CEnvModifier E;
				E.load(fs, ver);
				Modifiers.push_back(E);
			}
			id++;
		}
		FS.r_close(fs);
	}
	LoadLevelSpecificAmbients();
}

void CEnvironment::ModsUnload()
{
	Modifiers.clear();
}

void CEnvironment::LoadLevelSpecificAmbients()
{
	const shared_str level_name = g_pGameLevel->name();

	string_path path;
	xr_strconcat(path, "environment\\ambients\\", level_name.c_str(), ".ltx");

	string_path full_path;
	FS.update_path(full_path, "$game_config$", path);
	CInifile* level_ambients = xr_new<CInifile>(full_path, TRUE, TRUE, FALSE);

	for (CEnvAmbient* envAmbient : Ambients)
	{
		shared_str section_name = envAmbient->name();

		// Choose a source ini file
		CInifile* source = (level_ambients && level_ambients->section_exist(section_name)) ? level_ambients : m_ambients_config;

		// Check and reload if needed
		if (xr_strcmp(envAmbient->get_ambients_config_filename().c_str(), source->fname()))
		{
			envAmbient->Destroy();
			envAmbient->Load(*source, *m_sound_channels_config, *m_effects_config, section_name);
		}
	}
	xr_delete(level_ambients);
}

CEnvDescriptor* CEnvironment::CreateDescriptor(shared_str const& identifier, CInifile* config)
{
	CEnvDescriptor*	result = xr_new<CEnvDescriptor>(identifier);
	if (config)
		result->Load(*this, *config);

	return result;
}

void CEnvironment::LoadWeathers()
{
	if (!WeatherCycles.empty())
		return;

	xr_vector<LPSTR>* file_list = FS.file_list_open("$game_weathers$", "");
	VERIFY(file_list);

	for (LPSTR &fileName : *file_list)
	{
		u32							length = xr_strlen(fileName);

		VERIFY(length >= 4);
		VERIFY(fileName[length - 4] == '.');
		VERIFY(fileName[length - 3] == 'l');
		VERIFY(fileName[length - 2] == 't');
		VERIFY(fileName[length - 1] == 'x');

		const u32 new_length = length - 4;
		char* pIdentifier			= new char[new_length + 1];
		std::memcpy(pIdentifier, fileName, new_length * sizeof(char));
		pIdentifier[new_length]		= '\0';
		EnvVec& env					= WeatherCycles[pIdentifier];

		string_path file_name;
		FS.update_path				(file_name, "$game_weathers$", pIdentifier);
		xr_strcat					(file_name, ".ltx");
		CInifile*					config = CInifile::Create(file_name);

		typedef CInifile::Root		sections_type;
		sections_type&				sections = config->sections();

		env.reserve					(sections.size());

		for (auto& it : sections) 
		{
			CEnvDescriptor* object = CreateDescriptor(it.second->Name, config);
			env.push_back(object);
		}
		CInifile::Destroy(config);

		xr_delete(pIdentifier);
	}
	FS.file_list_close(file_list);

	// Sorting weather envs
	for (auto& envDesc : WeatherCycles)
	{
		R_ASSERT3(envDesc.second.size() > 1, "Environment in weather must >=2", envDesc.first.data());
		std::sort(envDesc.second.begin(), envDesc.second.end(), sort_env_etl_pred);
	}
	R_ASSERT2(!WeatherCycles.empty(), "Empty weathers.");
	SetWeather((*WeatherCycles.begin()).first);
}

void CEnvironment::LoadWeatherEffects()
{
	if (!WeatherFXs.empty())
		return;

	typedef xr_vector<LPSTR>		file_list_type;
    file_list_type*					pfile_list = FS.file_list_open("$game_weather_effects$", "");
	VERIFY							(pfile_list);
	file_list_type&					file_list = *pfile_list;

	for (char* weatherEffectFileName : file_list)
	{
		u32							length = xr_strlen(weatherEffectFileName);
		VERIFY						(length >= 4);
		VERIFY						((weatherEffectFileName)[length - 4] == '.');
		VERIFY						((weatherEffectFileName)[length - 3] == 'l');
		VERIFY						((weatherEffectFileName)[length - 2] == 't');
		VERIFY						((weatherEffectFileName)[length - 1] == 'x');
		u32							new_length = length - 4;
		char* pIdentifier = new char[new_length + 1];
        std::memcpy(pIdentifier, weatherEffectFileName, new_length*sizeof(char));
		pIdentifier[new_length]		= 0;
		EnvVec& env					= WeatherFXs[pIdentifier];

		string_path					file_name;
		FS.update_path				(file_name, "$game_weather_effects$", pIdentifier);
		xr_strcat					(file_name, ".ltx");
		CInifile*					config = CInifile::Create(file_name);

		typedef CInifile::Root		sections_type;
		sections_type&				sections = config->sections();

		env.reserve					(sections.size() + 2);
		env.push_back				(CreateDescriptor("00:00:00", false));

		for (auto[name, unused] : sections)
		{
			CEnvDescriptor* object = CreateDescriptor(name, config);
			env.push_back(object);
		}

		xr_delete(pIdentifier);
		CInifile::Destroy			(config);

		env.push_back				(CreateDescriptor("24:00:00", false));
		env.back()->exec_time_loaded = DAY_LENGTH;

	}

	FS.file_list_close(pfile_list);

	// sorting weather envs
	for (auto[SharedString, Vector] : WeatherFXs)
	{
		R_ASSERT3(Vector.size() > 1, "Must be >=2 environments in weather", SharedString.c_str());
		std::sort(Vector.begin(), Vector.end(), sort_env_etl_pred);
	}
}

void CEnvironment::Load()
{
	if (!CurrentEnv)
		CreateMixer();

	m_pRender->OnLoad();
	if (!eff_Rain)    		eff_Rain 		= xr_new<CEffectRain>();
	if (!eff_LensFlare)		eff_LensFlare 	= xr_new<CLensFlare>();
	if (!eff_Thunderbolt)	eff_Thunderbolt	= xr_new<CEffect_Thunderbolt>();
	
	LoadWeathers();
	LoadWeatherEffects();
}

void CEnvironment::Unload()
{
	// Clear weathers
	for (auto &wCycle : WeatherCycles)
	{
		for (CEnvDescriptor* envDesc : wCycle.second)
			xr_delete(envDesc);
	}
	WeatherCycles.clear();

	// Clear weather effects
	for (auto &wFX : WeatherFXs)
	{
		for (CEnvDescriptor* envDesc : wFX.second)
			xr_delete(envDesc);
	}
	WeatherFXs.clear();

	// Clear ambients
	for (CEnvAmbient* envAmbient : Ambients)
	{
		xr_delete(envAmbient);
	}
	Ambients.clear();

	// misc
	xr_delete			(eff_Rain);
	xr_delete			(eff_LensFlare);
	xr_delete			(eff_Thunderbolt);
	CurrentWeather		= nullptr;
	CurrentWeatherName	= nullptr;
	CurrentEnv->Clear	();
	Invalidate			();

	m_pRender->OnUnload	();
}

void CEnvironment::CreateMixer()
{
	VERIFY(!CurrentEnv);
	CurrentEnv = xr_new<CEnvDescriptorMixer>("00:00:00");
}

void CEnvironment::DestroyMixer()
{
	xr_delete(CurrentEnv);
}

CThunderboltDesc* CEnvironment::ThunderboltDescription(CInifile& config, shared_str const& section)
{
	CThunderboltDesc* result = xr_new<CThunderboltDesc>();
	result->Load(config, section);

	return result;
}

CThunderboltCollection* CEnvironment::ThunderboltCollection(CInifile* pIni, CInifile* thunderbolts, LPCSTR section)
{
	CThunderboltCollection*	result = xr_new<CThunderboltCollection>();
	result->Load(pIni, thunderbolts, section);

	return result;
}

CThunderboltCollection* CEnvironment::ThunderboltCollection(xr_vector<CThunderboltCollection*>& collection,  shared_str const& id)
{
	for (CThunderboltCollection* tbCollection : collection)
	{
		if (tbCollection->section == id)
			return tbCollection;
	}

	NODEFAULT;
#ifdef DEBUG
	return nullptr;
#endif
}

CLensFlareDescriptor* CEnvironment::AddFlare(xr_vector<CLensFlareDescriptor*>& collection, shared_str const& id)
{
	for (CLensFlareDescriptor* flareDesc : collection)
	{
		if (flareDesc->section == id)
			return flareDesc;
	}

	CLensFlareDescriptor* result = xr_new<CLensFlareDescriptor>();
	result->Load(m_suns_config, id.c_str());
	collection.push_back(result);
	
	return result;
}