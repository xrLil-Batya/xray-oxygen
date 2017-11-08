#pragma once

#include "interactive_motion.h"

class XRPHYSICS_API imotion_velocity: public interactive_motion
{
	typedef			interactive_motion inherited;
	virtual	void	move_update	(  );
	virtual	void	collide		(  );
	virtual	void	state_end	(  );
	virtual	void	state_start (  );
};
