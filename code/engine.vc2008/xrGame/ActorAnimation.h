#ifndef _Actor_Animation_H
#define _Actor_Animation_H
#pragma once

// animation state constants
//-------------------------------------------------------------------------------
#define _Fwd			(mcFwd)
#define _Back			(mcBack)
#define _LStr			(mcLStrafe)
#define _RStr			(mcRStrafe)
#define _FwdLStr		(mcFwd|mcLStrafe)
#define _FwdRStr		(mcFwd|mcRStrafe)
#define _BackLStr		(mcBack|mcLStrafe)
#define _BackRStr		(mcBack|mcRStrafe)
	
#define _AFwd			(mcAccel|mcFwd)
#define _ABack			(mcAccel|mcBack)
#define _ALStr			(mcAccel|mcLStrafe)
#define _ARStr			(mcAccel|mcRStrafe)
#define _AFwdLStr		(mcAccel|mcFwd|mcLStrafe)
#define _AFwdRStr		(mcAccel|mcFwd|mcRStrafe)
#define _ABackLStr		(mcAccel|mcBack|mcLStrafe)
#define _ABackRStr		(mcAccel|mcBack|mcRStrafe)
// 
#define _Jump			(mcJump)
//-------------------------------------------------------------------------------

#endif //_Actor_Animation_H
