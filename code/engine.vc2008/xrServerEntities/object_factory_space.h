////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_space.h
//	Created 	: 30.06.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory space
////////////////////////////////////////////////////////////////////////////

#ifndef object_factory_spaceH
#define object_factory_spaceH

#pragma once

#ifndef XRGAME_EXPORTS
#	define NO_XR_GAME
#endif

class CSE_Abstract;

namespace ObjectFactory {

#ifndef NO_XR_GAME
	using CLIENT_BASE_CLASS = DLL_Pure;
#endif
	using SERVER_BASE_CLASS = CSE_Abstract;

#ifndef NO_XR_GAME
	using CLIENT_SCRIPT_BASE_CLASS = DLL_Pure;
#endif
	using SERVER_SCRIPT_BASE_CLASS = CSE_Abstract;
};

#endif