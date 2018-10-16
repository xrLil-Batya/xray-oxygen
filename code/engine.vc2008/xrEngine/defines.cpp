#include "stdafx.h"

ENGINE_API bool bDebug	= FALSE;

// Video
u32			psCurrentVidMode[2] = {1024,768};
// release version always has "mt_*" enabled
ENGINE_API Flags32		psDeviceFlags		= {rsDetails|rsDrawStatic|rsDrawDynamic};
// textures 
int			psTextureLOD		= 1;
