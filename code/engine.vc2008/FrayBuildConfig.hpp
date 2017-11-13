#pragma once

/* xrCore */
#define	LOG_TIME_PRECISE // (by alpet & RvP)

/* Scripts */
//#define SPAWN_ANTIFREEZE /* spread spawn of game objects thoughout multiple frames to prevent lags (by alpet) */
//#define LUACP_API		// attaching luaicp.dll

/* xrRender */
#define R34_USE_FIRSTMIPLEVEL //activate: LoadInfo.FirstMipLevel = img_loaded_lod;

/* xrGame */
#define WPN_AUTORELOAD // Enable autoreload
//#define DYNAMIC_SUN_MOVEMENT // Use dynamic sun movement
//#define NEW_SLOTS // Add knife slot and binocular slot in inventory
//#define ASPAWN_CROW // Spawn crows over actor
//#define NEW_ANIMS_WPN // Enable anm_..._crouch, anm_reload_empty