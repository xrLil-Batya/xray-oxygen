#ifndef COMMON_H
#define COMMON_H
#ifdef __INTELLISENSE__
#include "../toolbox/VSIntelliSenceForHLSL.h"
#endif
#include "shared\common.h"

#include "common_defines.h"
#include "common_policies.h"
#include "common_iostructs.h"
#include "common_samplers.h"
#include "common_cbuffers.h"
#include "common_functions.h"

//#define USE_SUPER_SPECULAR

#define USE_SUNMASK

#ifdef        USE_R2_STATIC_SUN
#  define xmaterial float(1.0h/4.h)
#else
#  define xmaterial float(L_material.w)
#endif
#endif
