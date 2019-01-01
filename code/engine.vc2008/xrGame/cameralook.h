#pragma once

#include "../xrEngine/CameraBase.h"

class CCameraLook	: public CCameraBase
{
	using inherited = CCameraBase;

	Fvector2		lim_zoom;
protected:
	float			dist, prev_d;
public:
					CCameraLook		( CObject* p, u32 flags=0);
	virtual			~CCameraLook	( );
	virtual void	Load			(LPCSTR section);
	virtual void	Move			( int cmd, float val=0, float factor=1.0f );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_dangle );

	virtual float	GetWorldYaw		( )	{ return -yaw;	};
	virtual float	GetWorldPitch	( )	{ return pitch; };
protected:
			void	 UpdateDistance	( Fvector& point );
};

class CCameraLook2	: public CCameraLook
{
public:
	static Fvector	m_cam_offset;
	ref_light enemy_spotlight;
protected:

	CObject*		m_locked_enemy;
	Fvector2		m_autoaim_inertion_yaw;
	Fvector2		m_autoaim_inertion_pitch;
	void			UpdateAutoAim	();
public:
					CCameraLook2	( CObject* p, u32 flags=0):CCameraLook(p, flags){m_locked_enemy=nullptr;	enemy_spotlight = ::Render->light_create(); enemy_spotlight->set_active(0);};
	virtual			~CCameraLook2	(){ enemy_spotlight.destroy();}
	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_dangle );
	virtual void	Load			(LPCSTR section);
};