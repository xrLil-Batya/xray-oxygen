#include "stdafx.h"
#include "ai_space.h"
#include "object_factory.h"
#include "ai/monsters/ai_monster_squad_manager.h"
#include "..\xrEngine\string_table.h"

#include "entity_alive.h"
#include "ui/UIInventoryUtilities.h"
#include "../xrUICore/UIXmlInit.h"
#include "../xrUICore/UItextureMaster.h"

#include "InfoPortion.h"
#include "PhraseDialog.h"
#include "GameTask.h"

#include "character_info.h"
#include "specific_character.h"
#include "character_community.h"
#include "monster_community.h"
#include "character_rank.h"
#include "character_reputation.h"

#include "../xrEngine/profiler.h"

#include "sound_collection_storage.h"
#include "relation_registry.h"

using STORY_PAIRS = xr_vector<std::pair<shared_str,int> >;
extern STORY_PAIRS								story_ids;
extern STORY_PAIRS								spawn_story_ids;

extern void ENGINE_API clean_wnd_rects				();
extern void UI_API CreateUIGeom						();
extern void UI_API DestroyUIGeom					();
extern void InitHudSoundSettings					();

#include "../xrEngine/IGame_Persistent.h"
void init_game_globals()
{
	CreateUIGeom									();
	InitHudSoundSettings							();
	CPhraseDialog::InitInternal					    ();
	InventoryUtilities::CreateShaders			    ();
	CCharacterInfo::InitInternal					();
	CSpecificCharacter::InitInternal				();
	CCharacterCommunity::InitInternal				();
	CHARACTER_RANK::InitInternal					();
	CHARACTER_REPUTATION::InitInternal				();
	MONSTER_COMMUNITY::InitInternal					();
}

extern CUIXml*	g_uiSpotXml;
extern CUIXml*	pWpnScopeXml;

extern void destroy_lua_wpn_params	();

void clean_game_globals()
{
	destroy_lua_wpn_params							();
	// destroy object factory
	xr_delete										(g_object_factory);
	// destroy monster squad global var
	xr_delete										(g_monster_squad);

	story_ids.clear									();
	spawn_story_ids.clear							();

	CPhraseDialog::DeleteSharedData					();
	CPhraseDialog::DeleteIdToIndexData				();
	
	InventoryUtilities::DestroyShaders				();

	CCharacterInfo::DeleteSharedData				();
	CCharacterInfo::DeleteIdToIndexData				();
	
	CSpecificCharacter::DeleteSharedData			();
	CSpecificCharacter::DeleteIdToIndexData			();

	CCharacterCommunity::DeleteIdToIndexData		();
	CHARACTER_RANK::DeleteIdToIndexData				();
	CHARACTER_REPUTATION::DeleteIdToIndexData		();
	MONSTER_COMMUNITY::DeleteIdToIndexData			();


	//static shader for blood
	CEntityAlive::UnloadBloodyWallmarks				();
	CEntityAlive::UnloadFireParticles				();
	//очищение памяти таблицы строк
	CStringTable::Destroy							();
	// Очищение таблицы цветов
	CUIXmlInit::DeleteColorDefs						();
	// Очищение таблицы идентификаторов рангов и отношений сталкеров
	InventoryUtilities::ClearCharacterInfoStrings	();

	xr_delete										(g_sound_collection_storage);
	
#ifdef DEBUG
	xr_delete										(g_profiler);
#endif

	SRelationRegistry::clear_relation_registry		();
	clean_wnd_rects									();
	xr_delete										(g_uiSpotXml);
	DestroyUIGeom									();
	xr_delete										(pWpnScopeXml);
	CUITextureMaster::FreeTexInfo					();
}