#pragma once

#include "r_constants.h"

#if defined(USE_DX10) || defined(USE_DX11)
#include "../xrRenderDX10/dx10r_constants_cache.h"
#else	//	USE_DX10
#include "../xrRenderDX9/dx9r_constants_cache.h"
#endif	//	USE_DX10
