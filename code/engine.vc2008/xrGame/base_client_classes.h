////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes.h
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrScripts/export/script_export_space.h"

class DLL_Pure;
class ISpatial;
class ISheduled;
class IRenderable;
class ICollidable;
class CObject;
class IRenderVisual;
class FHierrarhyVisual;
class CBlend;
class IKinematics;
class IKinematicsAnimated;
class CPatrolPoint;
class CPatrolPath;

using DLL_PureScript = class_exporter<DLL_Pure>;
add_to_type_list(DLL_PureScript)
#undef script_type_list
#define script_type_list save_type_list(DLL_PureScript)

//typedef class_exporter<ISpatial>	ISpatialScript;
//add_to_type_list(ISpatialScript)
//#undef script_type_list
//#define script_type_list save_type_list(ISpatialScript)

using ISheduledScript = class_exporter<ISheduled>;
add_to_type_list(ISheduledScript)
#undef script_type_list
#define script_type_list save_type_list(ISheduledScript)

using IRenderableScript = class_exporter<IRenderable>;
add_to_type_list(IRenderableScript)
#undef script_type_list
#define script_type_list save_type_list(IRenderableScript)

using ICollidableScript = class_exporter<ICollidable>;
add_to_type_list(ICollidableScript)
#undef script_type_list
#define script_type_list save_type_list(ICollidableScript)

using CObjectScript = class_exporter<CObject>;
add_to_type_list(CObjectScript)
#undef script_type_list
#define script_type_list save_type_list(CObjectScript)

using CBlendScript = class_exporter<CBlend>;
add_to_type_list(CBlendScript)
#undef script_type_list
#define script_type_list save_type_list(CBlendScript)

using IRender_VisualScript = class_exporter<IRenderVisual>;
add_to_type_list(IRender_VisualScript)
#undef script_type_list
#define script_type_list save_type_list(IRender_VisualScript)
/*
typedef class_exporter<FHierrarhyVisual>	FHierrarhyVisualScript;
add_to_type_list(FHierrarhyVisualScript)
#undef script_type_list
#define script_type_list save_type_list(FHierrarhyVisualScript)

typedef class_exporter<IKinematics>	IKinematicsScript;
add_to_type_list(IKinematicsScript)
#undef script_type_list
#define script_type_list save_type_list(IKinematicsScript)
*/

using IKinematicsAnimatedScript = class_exporter<IKinematicsAnimated>;
add_to_type_list(IKinematicsAnimatedScript)
#undef script_type_list
#define script_type_list save_type_list(IKinematicsAnimatedScript)

class CPatrolPointScript { // TODO mb need implement this as GSC? or relocate that class to better place?
public:
	static LPCSTR getName(CPatrolPoint*);
	static void   setName(CPatrolPoint*, LPCSTR);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPatrolPointScript)
#undef script_type_list
#define script_type_list save_type_list(CPatrolPointScript)

using CPatrolPathScript = class_exporter<CPatrolPath>;
add_to_type_list(CPatrolPathScript)
#undef script_type_list
#define script_type_list save_type_list(CPatrolPathScript) 