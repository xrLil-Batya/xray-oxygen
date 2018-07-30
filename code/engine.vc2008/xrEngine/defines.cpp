#include "stdafx.h"

ENGINE_API bool bDebug	= FALSE;

// Video
u32			psCurrentVidMode[2] = {1024,768};
// release version always has "mt_*" enabled
Flags32		psDeviceFlags		= {rsDetails|mtPhysics|mtSound|mtNetwork|rsDrawStatic|rsDrawDynamic};
// textures 
int			psTextureLOD		= 1;
