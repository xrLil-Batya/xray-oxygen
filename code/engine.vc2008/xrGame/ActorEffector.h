#pragma once

#include "CameraEffector.h"

class CObjectAnimator;
class CEffectorController;
class CActor;

class GAME_API CActorCameraManager : public CCameraManager
{
	using inherited = CCameraManager;

	SCamEffectorInfo		m_cam_info_hud;

protected:
	virtual void			UpdateCamEffectors		();
	virtual bool			ProcessCameraEffector	(CEffectorCam* eff);

public:
							CActorCameraManager		() : inherited(false) {}
	virtual					~CActorCameraManager	() {}

	IC void					hud_camera_Matrix		(Fmatrix& M){M.set(m_cam_info_hud.r, m_cam_info_hud.n, m_cam_info_hud.d, m_cam_info_hud.p);}
};

using GET_KOEFF_FUNC = xrDelegate<float()>;

void GAME_API AddEffector		(CActor* A, int type, const shared_str& sect_name);
void GAME_API AddEffector		(CActor* A, int type, const shared_str& sect_name, float factor);
void GAME_API AddEffector		(CActor* A, int type, const shared_str& sect_name, GET_KOEFF_FUNC);
void GAME_API AddEffector		(CActor* A, int type, const shared_str& sect_name, CEffectorController*);
void GAME_API RemoveEffector		(CActor* A, int type);

class GAME_API CEffectorController
{
protected:
	CEffectorCam*				m_ce;
	CEffectorPP*				m_pe;

public:
								CEffectorController		() : m_ce(nullptr), m_pe(nullptr) {}
	virtual						~CEffectorController	();

				void			SetPP					(CEffectorPP* p)	{m_pe = p;}
				void			SetCam					(CEffectorCam* p)	{m_ce = p;}
	virtual		BOOL			Valid					()					{return m_ce || m_pe;};
	virtual		float __stdcall	GetFactor				()					= 0;
};

class GAME_API CAnimatorCamEffector : public CEffectorCam
{
	bool				m_bCyclic;

protected:
	using inherited =				CEffectorCam;
	virtual bool		Cyclic					() const		{return m_bCyclic;}
	CObjectAnimator*							m_objectAnimator;

public:
	bool				m_bAbsolutePositioning;
	float				m_fov;

						CAnimatorCamEffector	();
	virtual				~CAnimatorCamEffector	();
			void		Start					(LPCSTR fn);
	virtual BOOL		ProcessCam				(SCamEffectorInfo& info);
			void		SetCyclic				(bool b)				{m_bCyclic = b;}
	virtual	BOOL		Valid					();
			float		GetAnimatorLength		()						{return fLifeTime;};

	virtual bool		AbsolutePositioning		()						{return m_bAbsolutePositioning;}
};

class GAME_API CAnimatorCamEffectorScriptCB : public CAnimatorCamEffector
{
	using inherited = 	CAnimatorCamEffector;

	shared_str			cb_name;

public:
	CAnimatorCamEffectorScriptCB				(LPCSTR _cb)				{cb_name =_cb;};
	virtual	BOOL		Valid					();
	virtual BOOL		AllowProcessingIfInvalid()							{return m_bAbsolutePositioning;}
	virtual	void		ProcessIfInvalid		(SCamEffectorInfo& info);
};

class GAME_API CAnimatorCamLerpEffector : public CAnimatorCamEffector
{
protected:
	using inherited =				CAnimatorCamEffector;
	GET_KOEFF_FUNC									m_func;

public:
			void		SetFactorFunc				(GET_KOEFF_FUNC f)	{m_func = f;}
	virtual BOOL		ProcessCam					(SCamEffectorInfo& info);
};

class GAME_API CAnimatorCamLerpEffectorConst : public CAnimatorCamLerpEffector
{
protected:
	float				m_factor;

public:
						CAnimatorCamLerpEffectorConst	();
	void				SetFactor						(float v)		{m_factor = v; clamp(m_factor, 0.0f, 1.0f);}
	float	__stdcall	GetFactor						()				{return m_factor;}
};

class GAME_API CCameraEffectorControlled : public CAnimatorCamLerpEffector
{
	CEffectorController*		m_controller;

public:
						CCameraEffectorControlled		(CEffectorController* c);
	virtual				~CCameraEffectorControlled		();
	virtual BOOL		Valid							();
};

class GAME_API SndShockEffector : public CEffectorController
{
	using inherited = CEffectorController;

public:
	float						m_snd_length;	//ms
	float						m_cur_length;	//ms
	float						m_stored_volume;
	float						m_end_time;
	float						m_life_time;
	CActor*						m_actor;

public:
								SndShockEffector	();
	virtual						~SndShockEffector	();
	void						Start				(CActor* A, float snd_length, float power);
	void						Update				();

	virtual		BOOL			Valid				();
				BOOL			InWork				();
	virtual		float __stdcall	GetFactor			();
};

//////////////////////////////////////////////////////////////////////////
class GAME_API CControllerPsyHitCamEffector : public CEffectorCam
{
	using inherited = CEffectorCam;
	
	float				m_time_total;
	float				m_time_current;
	Fvector				m_dangle_target;
	Fvector				m_dangle_current;
	Fvector				m_position_source;
	Fvector				m_direction;
	float				m_distance;
	float				m_base_fov;
	float				m_dest_fov;

public:
						CControllerPsyHitCamEffector	(ECamEffectorType type, const Fvector &src_pos, const Fvector &target_pos, 
														float time, float base_fov, float dest_fov);
	virtual BOOL		ProcessCam						(SCamEffectorInfo& info);
};
//////////////////////////////////////////////////////////////////////////
