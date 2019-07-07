#pragma once

class IKinematics;
class IKinematicsAnimated;
class IParticleCustom;
struct vis_data;

class IRenderVisual
{
public:
	bool bIgnoreOpt;
	IRenderVisual() : bIgnoreOpt(false) {} ;
	virtual ~IRenderVisual() {;}

	virtual vis_data&		getVisData() = 0;
	virtual u32					getType() = 0;

	virtual shared_str		getDebugName() = 0;

	virtual	IKinematics*		dcast_PKinematics			()				{ return 0;	}
	virtual	IKinematicsAnimated*	dcast_PKinematicsAnimated	()				{ return 0;	}
	virtual IParticleCustom*		dcast_ParticleCustom		()				{ return 0;	}
};