#pragma once
#include "CameraBase.h"

class ENGINE_API CCameraFirstEye : public CCameraBase
{
	using inherited = CCameraBase;
	Fvector			lookat_point;
	bool			lookat_active;
	void			UpdateLookat	();
public:
					CCameraFirstEye	( CObject* p, u32 flags=0);
	virtual			~CCameraFirstEye( );

	virtual void	Load			(LPCSTR section);
	virtual void	Move			( int cmd, float val=0, float factor=1.0f );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_angle );

	virtual float	GetWorldYaw		( )	{ return -yaw;	};
	virtual float	GetWorldPitch	( )	{ return pitch; };
			void	LookAtPoint		(Fvector p){lookat_point=p;lookat_active=true;}
};