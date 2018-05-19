#include "stdafx.h"

ENGINE_API bool bDebug	= FALSE;

// Video
//. u32			psCurrentMode		= 1024;
u32			psCurrentVidMode[2] = {1024,768};
u32			psCurrentBPP		= 32;
// release version always has "mt_*" enabled
Flags32		psDeviceFlags		= {rsDetails|mtPhysics|mtSound|mtNetwork|rsDrawStatic|rsDrawDynamic|rsRefresh60hz};
// textures 
int			psTextureLOD		= 1;
