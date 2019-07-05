#pragma once
ENGINE_API	extern bool			bDebug;

#define _RELEASE(x)			{ if(x) { (x)->Release(); (x)=NULL; } }
#define _SHOW_REF(msg, x)   { if(x) { x->AddRef(); Msg("%s%d", msg, u32(x->Release()));}}

// textures
ENGINE_API extern	int		psTextureLOD;

// psDeviceFlags
enum: unsigned long
{
	rsFullscreen					= (1<<0),
	rsClearBB						= (1<<1),
	rsVSync							= (1<<2),
	rsTripleBuffering				= (1<<4),
	rsStatistic						= (1<<5),
	rsDetails						= (1<<6),
	rsConstantFPS					= (1<<8),
	rsDrawStatic					= (1<<9),
	rsDrawDynamic					= (1<<10),
	rsDisableObjectsAsCrows			= (1<<11),

	rsOcclusionDraw					= (1<<12),
	rsOcclusionStats				= (1<<13),

	rsGameProfiler					= (1<<14),
	rsScheduleProfiler				= (1<<15),

	rsCameraPos						= (1<<18),
	rsR2							= (1<<20),
	rsR4							= (1<<22), // was reserved to Editor
	rsDrawFPS						= (1<<23), // was reserved to Editor
	rsHWInfo						= (1<<24),
	rsEnviromentInfo				= (1<<25)
	// 22-32 bit - reserved to Editor
};

ENGINE_API extern	u32			psCurrentVidMode[];
ENGINE_API extern	u32			ps_r_RefreshHZ;
ENGINE_API extern	Flags32		psDeviceFlags;

// game path definition
#define _game_data_				"$game_data$"
#define _game_textures_			"$game_textures$"
#define _game_levels_			"$game_levels$"
#define _game_sounds_			"$game_sounds$"
#define _game_meshes_			"$game_meshes$"
#define _game_shaders_			"$game_shaders$"
#define _game_config_			"$game_congif$"

// editor path definition
#define _server_root_		    "$server_root$"
#define _server_data_root_	    "$server_data_root$"
#define _local_root_		    "$local_root$"
#define _import_			    "$import$"
#define _sounds_			    "$sounds$"
#define _textures_			    "$textures$"
#define _objects_			    "$objects$"
#define _maps_				    "$maps$"
#define _temp_				    "$temp$"
#define _omotion_			    "$omotion$"
#define _omotions_			    "$omotions$"
#define _smotion_			    "$smotion$"
#define _detail_objects_	    "$detail_objects$"
