#include "stdafx.h"
#include "../xrEngine/xr_ioconsole.h"
#include "../xrEngine/xr_ioc_cmd.h"
#include "../xrEngine/xr_ioc_cmd_ex.h"
#include "../xrEngine/customhud.h"
#include "../xrEngine/fdemorecord.h"
#include "../xrEngine/fdemoplay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "level.h"
#include "ai_debug.h"
#include "alife_simulator.h"

#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "script_process.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "../xrphysics/iphworld.h"
#include "..\xrEngine\string_table.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "../xrEngine/date_time.h"
#include "UIGame.h"
#include "ui/UIActorMenu.h"
#include "../xrUICore/UIStatic.h"
#include "zone_effector.h"
#include "GameTask.h"
#include "../xrUICore/MainMenu.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "cameralook.h"
#include "character_hit_animations_params.h"
#include "inventory_upgrade_manager.h"
#include "../xrCore/FS.h"
#include "../xrCore/LocatorAPI.h"
#include "UIZoneMap.h"
#include "ai_debug_variables.h"
#include "../xrphysics/console_vars.h"
#include "HudItem.h"
#include "attachable_item.h"
#include "attachment_owner.h"
#include "InventoryOwner.h"
#include "Inventory.h"
#include "GamePersistent.h"

ENGINE_API extern string_path g_last_saved_game;

#ifdef DEBUG
	extern float air_resistance_epsilon;
#endif // #ifdef DEBUG
float minimap_zoom_factor =1.0f;
extern	u64		g_qwStartGameTime;
extern 	u32 	hud_adj_mode;

ENGINE_API extern  float   psHUD_FOV_def;
extern	float	psSqueezeVelocity;
extern	int		psLUA_GCSTEP;

extern	int		x_m_x;
extern	int		x_m_z;
extern	int		g_dwInputUpdateDelta	;
extern	ESingleGameDifficulty g_SingleGameDifficulty;
extern UI_API BOOL g_show_wnd_rect2;
//-----------------------------------------------------------
extern	float	g_fTimeFactor;
extern	BOOL	b_toggle_weapon_aim;
//extern  BOOL	g_old_style_ui_hud;

extern float	g_smart_cover_factor;
extern int		g_upgrades_log;
extern float	g_smart_cover_animation_speed_factor;

extern	BOOL	g_ai_use_old_vision;
float			g_aim_predict_time = 0.44f;
int				g_keypress_on_start	= 1;

ENGINE_API extern float	g_console_sensitive;
extern	BOOL	g_ShowAnimationInfo;
//-----------------------------------------------------------
#ifdef DEBUG
#	include "console/con_debug.inl"
#endif // DEBUG
#include "console/con_physics.inl"
#include "console/con_alife.inl"
#include "console/con_game.inl"
//-----------------------------------------------------------

BOOL g_bCheckTime = FALSE;
int g_AI_inactive_time = 0;
Flags32 g_extraFeatures;

void CCC_RegisterCommands()
{
	// game
	CMD3(CCC_Mask, "g_binded_camera", &psActorFlags, AF_BINDED_CAMERA);
	CMD3(CCC_Mask, "g_aim_assist", &psActorFlags, AF_AIM_ASSIST);
	CMD3(CCC_Mask, "g_crouch_toggle", &psActorFlags, AF_CROUCH_TOGGLE);
	CMD3(CCC_Mask, "g_colored_feel", &psActorFlags, AF_COLORED_FEEL);
	CMD3(CCC_Mask, "hud_crosshair_collide", &psActorFlags, AF_CROSSHAIR_COLLIDE);
	CMD3(CCC_Mask, "hud_crosshair_inert", &psActorFlags, AF_CROSSHAIR_INERT);
	CMD1(CCC_GameDifficulty, "g_game_difficulty");

	CMD3(CCC_Mask, "g_backrun", &psActorFlags, AF_RUN_BACKWARD);

	// alife
#ifdef DEBUG
	CMD1(CCC_ALifePath, "al_path");		// build path

#endif // DEBUG

	CMD1(CCC_ALifeSave, "save");		// save game
	CMD1(CCC_ALifeLoadFrom, "load");		// load game from ...
	CMD1(CCC_LoadLastSave, "load_last_save");		// load last saved game from ...

	CMD1(CCC_FlushLog, "flush");		// flush log
	CMD1(CCC_ClearLog, "clear_log");


	CMD1(CCC_ALifeTimeFactor, "al_time_factor");		// set time factor
	CMD1(CCC_ALifeSwitchDistance, "al_switch_distance");		// set switch distance
	CMD1(CCC_ALifeProcessTime, "al_process_time");		// set process time
	CMD1(CCC_ALifeObjectsPerUpdate, "al_objects_per_update");		// set process time
	CMD1(CCC_ALifeSwitchFactor, "al_switch_factor");		// set switch factor

	CMD3(CCC_Mask, "hud_weapon", &psHUD_Flags, HUD_WEAPON);
	CMD3(CCC_Mask, "hud_info", &psHUD_Flags, HUD_INFO);
	CMD3(CCC_Mask, "hud_draw", &psHUD_Flags, HUD_DRAW);

	// hud
	psHUD_Flags.set(HUD_CROSSHAIR, true);
	psHUD_Flags.set(HUD_WEAPON, true);
	psHUD_Flags.set(HUD_DRAW, true);
	psHUD_Flags.set(HUD_INFO, true);

	CMD3(CCC_Mask, "hud_crosshair", &psHUD_Flags, HUD_CROSSHAIR);
	CMD3(CCC_Mask, "hud_crosshair_dist", &psHUD_Flags, HUD_CROSSHAIR_DIST);

	CMD4(CCC_Float, "hud_fov", &psHUD_FOV_def, 0.1f, 1.0f);
	CMD4(CCC_Float, "fov", &g_fov, 5.0f, 120.0f);

	// Demo
	CMD1(CCC_DemoPlay, "demo_play");
	CMD1(CCC_DemoRecord, "demo_record");
	CMD1(CCC_DemoRecordSetPos, "demo_set_cam_position");

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "ai_obstacles_avoiding", &psAI_Flags, aiObstaclesAvoiding);
	CMD3(CCC_Mask, "ai_obstacles_avoiding_static", &psAI_Flags, aiObstaclesAvoidingStatic);
	CMD3(CCC_Mask, "ai_use_smart_covers", &psAI_Flags, aiUseSmartCovers);
	CMD3(CCC_Mask, "ai_use_smart_covers_animation_slots", &psAI_Flags, (u32)aiUseSmartCoversAnimationSlot);
	CMD4(CCC_Float, "ai_smart_factor", &g_smart_cover_factor, 0.f, 1000000.f);
	CMD3(CCC_Mask, "ai_dbg_lua", &psAI_Flags, aiLua);
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD4(CCC_Integer, "lua_gcstep", &psLUA_GCSTEP, 1, 1000);
	CMD3(CCC_Mask, "ai_debug", &psAI_Flags, aiDebug);
	CMD3(CCC_Mask, "ai_dbg_brain", &psAI_Flags, aiBrain);
	CMD3(CCC_Mask, "ai_dbg_motion", &psAI_Flags, aiMotion);
	CMD3(CCC_Mask, "ai_dbg_frustum", &psAI_Flags, aiFrustum);
	CMD3(CCC_Mask, "ai_dbg_funcs", &psAI_Flags, aiFuncs);
	CMD3(CCC_Mask, "ai_dbg_alife", &psAI_Flags, aiALife);
	CMD3(CCC_Mask, "ai_dbg_goap", &psAI_Flags, aiGOAP);
	CMD3(CCC_Mask, "ai_dbg_goap_script", &psAI_Flags, aiGOAPScript);
	CMD3(CCC_Mask, "ai_dbg_goap_object", &psAI_Flags, aiGOAPObject);
	CMD3(CCC_Mask, "ai_dbg_cover", &psAI_Flags, aiCover);
	CMD3(CCC_Mask, "ai_dbg_anim", &psAI_Flags, aiAnimation);
	CMD3(CCC_Mask, "ai_dbg_vision", &psAI_Flags, aiVision);
	CMD3(CCC_Mask, "ai_dbg_monster", &psAI_Flags, aiMonsterDebug);
	CMD3(CCC_Mask, "ai_dbg_stalker", &psAI_Flags, aiStalker);
	CMD3(CCC_Mask, "ai_dbg_destroy", &psAI_Flags, aiDestroy);
	CMD3(CCC_Mask, "ai_dbg_serialize", &psAI_Flags, aiSerialize);
	CMD3(CCC_Mask, "ai_dbg_dialogs", &psAI_Flags, aiDialogs);
	CMD3(CCC_Mask, "ai_dbg_infoportion", &psAI_Flags, aiInfoPortion);

	CMD3(CCC_Mask, "ai_draw_game_graph", &psAI_Flags, aiDrawGameGraph);
	CMD3(CCC_Mask, "ai_draw_game_graph_stalkers", &psAI_Flags, aiDrawGameGraphStalkers);
	CMD3(CCC_Mask, "ai_draw_game_graph_objects", &psAI_Flags, aiDrawGameGraphObjects);
	CMD3(CCC_Mask, "ai_draw_game_graph_real_pos", &psAI_Flags, aiDrawGameGraphRealPos);

	CMD3(CCC_Mask, "ai_draw_visibility_rays", &psAI_Flags, aiDrawVisibilityRays);
	CMD3(CCC_Mask, "ai_animation_stats", &psAI_Flags, aiAnimationStats);

	// HIT ANIMATION
	CMD4(CCC_Float, "hit_anims_power", &ghit_anims_params.power_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_rotational_power", &ghit_anims_params.rotational_power_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_side_sensitivity_threshold", &ghit_anims_params.side_sensitivity_threshold, 0.0f, 10.0f);
	CMD4(CCC_Float, "hit_anims_channel_factor", &ghit_anims_params.anim_channel_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_block_blend", &ghit_anims_params.block_blend, 0.f, 1.f);
	CMD4(CCC_Float, "hit_anims_reduce_blend", &ghit_anims_params.reduce_blend, 0.f, 1.f);
	CMD4(CCC_Float, "hit_anims_reduce_blend_factor", &ghit_anims_params.reduce_power_factor, 0.0f, 1.0f);
	CMD4(CCC_Integer, "hit_anims_tune", &tune_hit_anims, 0, 1);
	/////////////////////////////////////////////HIT ANIMATION END////////////////////////////////////////////////////
	CMD1(CCC_DumpModelBones, "debug_dump_model_bones");

	CMD1(CCC_DrawGameGraphAll, "ai_draw_game_graph_all");
	CMD1(CCC_DrawGameGraphCurrent, "ai_draw_game_graph_current_level");
	CMD1(CCC_DrawGameGraphLevel, "ai_draw_game_graph_level");

	CMD4(CCC_Integer, "ai_dbg_inactive_time", &g_AI_inactive_time, 0, 1000000);

	CMD1(CCC_DebugNode, "ai_dbg_node");

	CMD1(CCC_ShowMonsterInfo, "ai_monster_info");
	CMD1(CCC_DebugFonts, "debug_fonts");

	CMD1(CCC_ShowAnimationStats, "ai_show_animation_stats");
#endif // DEBUG

	CMD1(CCC_TuneAttachableItem, "dbg_adjust_attachable_item");

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "ai_ignore_actor", &psAI_Flags, aiIgnoreActor);
#endif // MASTER_GOLD

	// Physics
	CMD1(CCC_PHFps, "ph_frequency");
	CMD1(CCC_PHIterations, "ph_iterations");
	CMD1(CCC_PHGravity, "ph_gravity");
	CMD4(CCC_FloatBlock, "ph_timefactor", &phTimefactor, 0.000001f, 1000.f);

#ifdef DEBUG
	CMD4(CCC_FloatBlock, "ph_break_common_factor", &ph_console::phBreakCommonFactor, 0.f, 1000000000.f);
	CMD4(CCC_FloatBlock, "ph_rigid_break_weapon_factor", &ph_console::phRigidBreakWeaponFactor, 0.f, 1000000000.f);
	CMD4(CCC_Integer, "ph_tri_clear_disable_count", &ph_console::ph_tri_clear_disable_count, 0, 255);
	CMD4(CCC_FloatBlock, "ph_tri_query_ex_aabb_rate", &ph_console::ph_tri_query_ex_aabb_rate, 1.01f, 3.f);
#endif // DEBUG
	CMD3(CCC_Mask, "g_no_clip", &psActorFlags, AF_NO_CLIP);
	CMD3(CCC_Mask, "rs_car_info", &psActorFlags, AF_CAR_INFO);
	CMD1(CCC_JumpToLevel, "jump_to_level");
	CMD3(CCC_Mask, "g_god", &psActorFlags, AF_GODMODE);
	CMD3(CCC_Mask, "g_unlimitedammo", &psActorFlags, AF_UNLIMITEDAMMO);
	CMD3(CCC_Mask, "cursor_instead_crosshair", &psActorFlags, AF_CUR_INS_CROS);
	CMD1(CCC_Script, "run_script");
	CMD1(CCC_ScriptCommand, "run_string");
	CMD3(CCC_Mask, "rs_show_cursor_pos", &psActorFlags, AF_SHOW_CURPOS);
	CMD3(CCC_Mask, "g_hardcore_mode", &psActorFlags, AF_HARDCORE);
    CMD3(CCC_Mask, "rs_wip", &psActorFlags, AF_WORKINPROGRESS);
    CMD3(CCC_Mask, "rs_clearskyinterface", &psActorFlags, AF_CLEARSKYINTERFACE);
    CMD3(CCC_Mask, "rs_showdate", &psActorFlags, AF_SHOWDATE);
	CMD1(CCC_TimeFactor, "time_factor");
	CMD1(CCC_Spawn, "g_spawn");
	CMD1(CCC_Spawn_to_inventory, "g_spawn_to_inventory");
	CMD1(CCC_Giveinfo, "g_info");
	CMD1(CCC_Disinfo, "d_info");
	CMD3(CCC_Mask, "g_autopickup", &psActorFlags, AF_AUTOPICKUP);
	CMD3(CCC_Mask, "g_dynamic_music", &psActorFlags, AF_DYNAMIC_MUSIC);
	CMD3(CCC_Mask, "g_important_save", &psActorFlags, AF_IMPORTANT_SAVE);
	CMD3(CCC_Mask, "ts_get_object_params", &psActorFlags, AF_GET_OBJECT_PARAMS);
	CMD3(CCC_Mask, "ts_show_boss_health", &psActorFlags, AF_SHOW_BOSS_HEALTH);
	CMD3(CCC_Mask, "g_right_shoulder", &psActorFlags, AF_RIGHT_SHOULDER);
	CMD3(CCC_Mask, "g_fp2_zoom_forced", &psActorFlags, AF_FP2ZOOM_FORCED);


#ifdef DEBUG

	CMD3(CCC_Mask, "dbg_draw_actor_alive", &dbg_net_Draw_Flags, dbg_draw_actor_alive);
	CMD3(CCC_Mask, "dbg_draw_actor_dead", &dbg_net_Draw_Flags, dbg_draw_actor_dead);
	CMD3(CCC_Mask, "dbg_draw_customzone", &dbg_net_Draw_Flags, dbg_draw_customzone);
	CMD3(CCC_Mask, "dbg_draw_teamzone", &dbg_net_Draw_Flags, dbg_draw_teamzone);
	CMD3(CCC_Mask, "dbg_draw_invitem", &dbg_net_Draw_Flags, dbg_draw_invitem);
	CMD3(CCC_Mask, "dbg_draw_actor_phys", &dbg_net_Draw_Flags, dbg_draw_actor_phys);
	CMD3(CCC_Mask, "dbg_draw_customdetector", &dbg_net_Draw_Flags, dbg_draw_customdetector);
	CMD3(CCC_Mask, "dbg_destroy", &dbg_net_Draw_Flags, dbg_destroy);
	CMD3(CCC_Mask, "dbg_draw_autopickupbox", &dbg_net_Draw_Flags, dbg_draw_autopickupbox);
	CMD3(CCC_Mask, "dbg_draw_rp", &dbg_net_Draw_Flags, dbg_draw_rp);
	CMD3(CCC_Mask, "dbg_draw_climbable", &dbg_net_Draw_Flags, dbg_draw_climbable);
	CMD3(CCC_Mask, "dbg_draw_skeleton", &dbg_net_Draw_Flags, dbg_draw_skeleton);


	CMD3(CCC_Mask, "dbg_draw_ph_contacts", &ph_dbg_draw_mask, phDbgDrawContacts);
	CMD3(CCC_Mask, "dbg_draw_ph_enabled_aabbs", &ph_dbg_draw_mask, phDbgDrawEnabledAABBS);
	CMD3(CCC_Mask, "dbg_draw_ph_intersected_tries", &ph_dbg_draw_mask, phDBgDrawIntersectedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_saved_tries", &ph_dbg_draw_mask, phDbgDrawSavedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_trace", &ph_dbg_draw_mask, phDbgDrawTriTrace);
	CMD3(CCC_Mask, "dbg_draw_ph_positive_tries", &ph_dbg_draw_mask, phDBgDrawPositiveTries);
	CMD3(CCC_Mask, "dbg_draw_ph_negative_tries", &ph_dbg_draw_mask, phDBgDrawNegativeTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_test_aabb", &ph_dbg_draw_mask, phDbgDrawTriTestAABB);
	CMD3(CCC_Mask, "dbg_draw_ph_tries_changes_sign", &ph_dbg_draw_mask, phDBgDrawTriesChangesSign);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_point", &ph_dbg_draw_mask, phDbgDrawTriPoint);
	CMD3(CCC_Mask, "dbg_draw_ph_explosion_position", &ph_dbg_draw_mask, phDbgDrawExplosionPos);
	CMD3(CCC_Mask, "dbg_draw_ph_statistics", &ph_dbg_draw_mask, phDbgDrawObjectStatistics);
	CMD3(CCC_Mask, "dbg_draw_ph_mass_centres", &ph_dbg_draw_mask, phDbgDrawMassCenters);
	CMD3(CCC_Mask, "dbg_draw_ph_death_boxes", &ph_dbg_draw_mask, phDbgDrawDeathActivationBox);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_app_pos", &ph_dbg_draw_mask, phHitApplicationPoints);
	CMD3(CCC_Mask, "dbg_draw_ph_cashed_tries_stats", &ph_dbg_draw_mask, phDbgDrawCashedTriesStat);
	CMD3(CCC_Mask, "dbg_draw_ph_car_dynamics", &ph_dbg_draw_mask, phDbgDrawCarDynamics);
	CMD3(CCC_Mask, "dbg_draw_ph_car_plots", &ph_dbg_draw_mask, phDbgDrawCarPlots);
	CMD3(CCC_Mask, "dbg_ph_ladder", &ph_dbg_draw_mask, phDbgLadder);
	CMD3(CCC_Mask, "dbg_draw_ph_explosions", &ph_dbg_draw_mask, phDbgDrawExplosions);
	CMD3(CCC_Mask, "dbg_draw_car_plots_all_trans", &ph_dbg_draw_mask, phDbgDrawCarAllTrnsm);
	CMD3(CCC_Mask, "dbg_draw_ph_zbuffer_disable", &ph_dbg_draw_mask, phDbgDrawZDisable);
	CMD3(CCC_Mask, "dbg_ph_obj_collision_damage", &ph_dbg_draw_mask, phDbgDispObjCollisionDammage);
	CMD_RADIOGROUPMASK2("dbg_ph_ai_always_phmove", &ph_dbg_draw_mask, phDbgAlwaysUseAiPhMove, "dbg_ph_ai_never_phmove", &ph_dbg_draw_mask, phDbgNeverUseAiPhMove);
	CMD3(CCC_Mask, "dbg_ph_ik", &ph_dbg_draw_mask, phDbgIK);
	CMD3(CCC_Mask, "dbg_ph_ik_off", &ph_dbg_draw_mask1, phDbgIKOff);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_goal", &ph_dbg_draw_mask, phDbgDrawIKGoal);
	CMD3(CCC_Mask, "dbg_ph_ik_limits", &ph_dbg_draw_mask, phDbgIKLimits);
	CMD3(CCC_Mask, "dbg_ph_character_control", &ph_dbg_draw_mask, phDbgCharacterControl);
	CMD3(CCC_Mask, "dbg_draw_ph_ray_motions", &ph_dbg_draw_mask, phDbgDrawRayMotions);
	CMD4(CCC_Float, "dbg_ph_vel_collid_damage_to_display", &dbg_vel_collid_damage_to_display, 0.f, 1000.f);
	CMD4(CCC_DbgBullets, "dbg_draw_bullet_hit", &g_bDrawBulletHit, 0, 1);
	CMD4(CCC_Integer, "dbg_draw_fb_crosshair", &g_bDrawFirstBulletCrosshair, 0, 1);
	CMD1(CCC_DbgPhTrackObj, "dbg_track_obj");
	CMD3(CCC_Mask, "dbg_ph_actor_restriction", &ph_dbg_draw_mask1, ph_m1_DbgActorRestriction);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_anims", &ph_dbg_draw_mask1, phDbgHitAnims);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_limits", &ph_dbg_draw_mask1, phDbgDrawIKLimits);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_predict", &ph_dbg_draw_mask1, phDbgDrawIKPredict);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_collision", &ph_dbg_draw_mask1, phDbgDrawIKCollision);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_shift_object", &ph_dbg_draw_mask1, phDbgDrawIKSHiftObject);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_blending", &ph_dbg_draw_mask1, phDbgDrawIKBlending);
	CMD1(CCC_DBGDrawCashedClear, "dbg_ph_cashed_clear");
	extern BOOL dbg_draw_character_bones;
	extern BOOL dbg_draw_character_physics;
	extern BOOL dbg_draw_character_binds;
	extern BOOL dbg_draw_character_physics_pones;
	extern BOOL ik_cam_shift;
	CMD4(CCC_Integer, "dbg_draw_character_bones", &dbg_draw_character_bones, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_physics", &dbg_draw_character_physics, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_binds", &dbg_draw_character_binds, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_physics_pones", &dbg_draw_character_physics_pones, FALSE, TRUE);

	CMD4(CCC_Integer, "ik_cam_shift", &ik_cam_shift, FALSE, TRUE);

	extern	float ik_cam_shift_tolerance;
	CMD4(CCC_Float, "ik_cam_shift_tolerance", &ik_cam_shift_tolerance, 0.f, 2.f);
	float ik_cam_shift_speed;
	CMD4(CCC_Float, "ik_cam_shift_speed", &ik_cam_shift_speed, 0.f, 1.f);
	extern	BOOL dbg_draw_doors;
	CMD4(CCC_Integer, "dbg_draw_doors", &dbg_draw_doors, FALSE, TRUE);

	extern 	BOOL dbg_draw_ragdoll_spawn;
	CMD4(CCC_Integer, "dbg_draw_ragdoll_spawn", &dbg_draw_ragdoll_spawn, FALSE, TRUE);
	extern BOOL debug_step_info;
	extern BOOL debug_step_info_load;
	CMD4(CCC_Integer, "debug_step_info", &debug_step_info, FALSE, TRUE);
	CMD4(CCC_Integer, "debug_step_info_load", &debug_step_info_load, FALSE, TRUE);
	extern BOOL debug_character_material_load;
	CMD4(CCC_Integer, "debug_character_material_load", &debug_character_material_load, FALSE, TRUE);
	extern XRPHYSICS_API BOOL dbg_draw_camera_collision;
	CMD4(CCC_Integer, "dbg_draw_camera_collision", &dbg_draw_camera_collision, FALSE, TRUE);
	extern XRPHYSICS_API float	camera_collision_character_skin_depth;
	extern XRPHYSICS_API float	camera_collision_character_shift_z;
	CMD4(CCC_FloatBlock, "camera_collision_character_shift_z", &camera_collision_character_shift_z, 0.f, 1.f);
	CMD4(CCC_FloatBlock, "camera_collision_character_skin_depth", &camera_collision_character_skin_depth, 0.f, 1.f);
	extern XRPHYSICS_API BOOL	dbg_draw_animation_movement_controller;
	CMD4(CCC_Integer, "dbg_draw_animation_movement_controller", &dbg_draw_animation_movement_controller, FALSE, TRUE);

	extern Flags32	dbg_track_obj_flags;
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_0", &dbg_track_obj_flags, dbg_track_obj_blends_bp_0);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_1", &dbg_track_obj_flags, dbg_track_obj_blends_bp_1);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_2", &dbg_track_obj_flags, dbg_track_obj_blends_bp_2);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_3", &dbg_track_obj_flags, dbg_track_obj_blends_bp_3);
	CMD3(CCC_Mask, "dbg_track_obj_blends_motion_name", &dbg_track_obj_flags, dbg_track_obj_blends_motion_name);
	CMD3(CCC_Mask, "dbg_track_obj_blends_time", &dbg_track_obj_flags, dbg_track_obj_blends_time);

	CMD3(CCC_Mask, "dbg_track_obj_blends_ammount", &dbg_track_obj_flags, dbg_track_obj_blends_ammount);
	CMD3(CCC_Mask, "dbg_track_obj_blends_mix_params", &dbg_track_obj_flags, dbg_track_obj_blends_mix_params);
	CMD3(CCC_Mask, "dbg_track_obj_blends_flags", &dbg_track_obj_flags, dbg_track_obj_blends_flags);
	CMD3(CCC_Mask, "dbg_track_obj_blends_state", &dbg_track_obj_flags, dbg_track_obj_blends_state);
	CMD3(CCC_Mask, "dbg_track_obj_blends_dump", &dbg_track_obj_flags, dbg_track_obj_blends_dump);

	CMD1(CCC_DbgVar, "dbg_var");

	extern float	dbg_text_height_scale;
	CMD4(CCC_FloatBlock, "dbg_text_height_scale", &dbg_text_height_scale, 0.2f, 5.f);
#endif


	CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
	CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
	CMD1(CCC_StartTimeSingle, "start_time_single");
	CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, 1000.0f);
	CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, Fvector().set(-1000, -1000, -1000), Fvector().set(1000, 1000, 1000));
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD1(CCC_Crash, "crash");
	CMD1(CCC_DumpObjects, "dump_all_objects");
	CMD3(CCC_String, "stalker_death_anim", dbg_stalker_death_anim, 32);
	CMD4(CCC_Integer, "death_anim_debug", &death_anim_debug, FALSE, TRUE);
	CMD4(CCC_Integer, "death_anim_velocity", &b_death_anim_velocity, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_imotion_draw_velocity", &dbg_imotion_draw_velocity, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_imotion_collide_debug", &dbg_imotion_collide_debug, FALSE, TRUE);

	CMD4(CCC_Integer, "dbg_imotion_draw_skeleton", &dbg_imotion_draw_skeleton, FALSE, TRUE);
	CMD4(CCC_Float, "dbg_imotion_draw_velocity_scale", &dbg_imotion_draw_velocity_scale, 0.0001f, 100.0f);

	CMD4(CCC_Integer, "dbg_dump_physics_step", &ph_console::g_bDebugDumpPhysicsStep, 0, 1);
	CMD1(CCC_InvUpgradesHierarchy, "inv_upgrades_hierarchy");
	CMD1(CCC_InvUpgradesCurItem, "inv_upgrades_cur_item");
	CMD4(CCC_Integer, "inv_upgrades_log", &g_upgrades_log, 0, 1);
	CMD1(CCC_InvDropAllItems, "inv_drop_all_items");

	CMD1(CCC_DumpInfos, "dump_infos");
	CMD1(CCC_DumpTasks, "dump_tasks");
	CMD1(CCC_DumpMap, "dump_map");
	CMD1(CCC_DumpCreatures, "dump_creatures");

#endif

	CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
	CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
	CMD1(CCC_StartTimeSingle, "start_time_single");
	CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, 1000.0f);
	CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, Fvector().set(-1000, -1000, -1000), Fvector().set(1000, 1000, 1000));
#endif

	CMD4(CCC_Float, "con_sensitive",     &g_console_sensitive, 0.01f, 1.0f);
	CMD4(CCC_Integer, "wpn_aim_toggle",  &b_toggle_weapon_aim, 0, 1);
//	CMD4(CCC_Integer,	"hud_old_style", &g_old_style_ui_hud, 0, 1);

	CMD4(CCC_Integer, "dbg_show_ani_info", &g_ShowAnimationInfo, 0, 1);
#ifdef DEBUG
	CMD4(CCC_Float, "ai_smart_cover_animation_speed_factor", &g_smart_cover_animation_speed_factor, .1f, 10.f);
	CMD4(CCC_Float, "air_resistance_epsilon", &air_resistance_epsilon, .0f, 1.f);

	CMD4(CCC_Integer, "show_wnd_rect_all", &g_show_wnd_rect2, 0, 1);

	extern BOOL dbg_moving_bones_snd_player;
	CMD4(CCC_Integer, "dbg_bones_snd_player", &dbg_moving_bones_snd_player, FALSE, TRUE);
#endif // #ifdef DEBUG

	CMD4(CCC_Integer, 	"g_sleep_time", 		&psActorSleepTime, 1, 24);
	CMD4(CCC_Integer, 	"ai_use_old_vision", 	&g_ai_use_old_vision, 0, 1);
	CMD4(CCC_Float, 	"ai_aim_predict_time", 	&g_aim_predict_time, 0.f, 10.f);

	extern float g_bullet_time_factor;
	CMD4(CCC_Float, "g_bullet_time_factor", &g_bullet_time_factor, 0.f, 10.f);


#ifdef DEBUG
	extern BOOL g_ai_dbg_sight;
	extern BOOL g_ai_aim_use_smooth_aim;

	CMD4(CCC_Integer, "ai_dbg_sight", &g_ai_dbg_sight, 0, 1);
	CMD4(CCC_Integer, "ai_aim_use_smooth_aim", &g_ai_aim_use_smooth_aim, 0, 1);
#endif // #ifdef DEBUG

	extern float g_ai_aim_min_speed;
	extern float g_ai_aim_min_angle;
	extern float g_ai_aim_max_angle;

	CMD4(CCC_Float, "ai_aim_min_speed", &g_ai_aim_min_speed, 0.f, 10.f*PI);
	CMD4(CCC_Float, "ai_aim_min_angle", &g_ai_aim_min_angle, 0.f, 10.f*PI);
	CMD4(CCC_Float, "ai_aim_max_angle", &g_ai_aim_max_angle, 0.f, 10.f*PI);

#ifdef DEBUG
	extern BOOL g_debug_doors;
	CMD4(CCC_Integer, "ai_debug_doors", &g_debug_doors, 0, 1);
#endif // #ifdef DEBUG

	*g_last_saved_game = 0;

	CMD3(CCC_String, "slot_0", g_quick_use_slots[0], 32);
	CMD3(CCC_String, "slot_1", g_quick_use_slots[1], 32);
	CMD3(CCC_String, "slot_2", g_quick_use_slots[2], 32);
	CMD3(CCC_String, "slot_3", g_quick_use_slots[3], 32);

	CMD4(CCC_Integer, "keypress_on_start", &g_keypress_on_start, 0, 1);

	// Oxy:
	CMD1(CCC_SetWeather, "set_weather");
	CMD3(CCC_MaskNoSave, "game_extra_ruck", &g_extraFeatures, GAME_EXTRA_RUCK);
    CMD3(CCC_MaskNoSave, "game_extra_monster_inventory", &g_extraFeatures, GAME_EXTRA_MONSTER_INVENTORY);
    CMD3(CCC_MaskNoSave, "game_extra_weapon_autoreload", &g_extraFeatures, GAME_EXTRA_WEAPON_AUTORELOAD);
    CMD3(CCC_MaskNoSave, "game_extra_dynamic_sun_movement", &g_extraFeatures, GAME_EXTRA_DYNAMIC_SUN);
    CMD3(CCC_MaskNoSave, "game_extra_spawn_antifreeze", &g_extraFeatures, GAME_EXTRA_SPAWN_ANTIFREEZE);
    CMD3(CCC_MaskNoSave, "game_extra_hold_to_pickup",	&g_extraFeatures, GAME_EXTRA_HOLD_TO_PICKUP);
    CMD3(CCC_MaskNoSave, "game_extra_soc_talk_wnd",		&g_extraFeatures, GAME_EXTRA_SOC_WND);
	CMD3(CCC_MaskNoSave, "game_extra_soc_minimap", &g_extraFeatures, GAME_EXTRA_OLD_SCHOOL_MINIMAP);
    CMD3(CCC_MaskNoSave, "game_extra_vertical_belts",	&g_extraFeatures, GAME_EXTRA_VERTICAL_BELTS);
    CMD3(CCC_MaskNoSave, "game_extra_thirst",			&g_extraFeatures, GAME_EXTRA_THIRST);
    CMD3(CCC_MaskNoSave, "game_extra_npc_grenade_up",	&g_extraFeatures, GAME_EXTRA_NPC_GRENADE_ATTAK_ALL);
    CMD3(CCC_MaskNoSave, "game_extra_lamps_immunity",	&g_extraFeatures, GAME_EXTRA_LAMP_IMMUNITY_SUPPORT);
	CMD3(CCC_MaskNoSave, "game_extra_polter_show_particles_on_dead", &g_extraFeatures, GAME_EXTRA_POLTER_SHOW_PARTICLES_ON_DEAD);
	CMD3(CCC_MaskNoSave, "game_extra_always_pickup", &g_extraFeatures, GAME_EXTRA_ALWAYS_PICKUP);

	CMD1(CCC_UIMapUpdate,"rs_minimap_zoom_factor");
	CMD4(CCC_U32,		"hud_adjust_mode", &hud_adj_mode, 0, 5); /// adjust mode support
}

void LoadGameExtraFeatures()
{
    g_extraFeatures.zero();
    string_path configFilePath;
    FS.update_path(configFilePath, "$game_config$", "GameExtra.ltx");

    string_path cmdLoadCfg;
	xr_strconcat( cmdLoadCfg, "cfg_load", " ", configFilePath);
    Console->Execute(cmdLoadCfg);

    Msg("Extra feature mask: %u", g_extraFeatures.get());
}
