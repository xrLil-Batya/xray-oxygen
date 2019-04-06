#pragma once

#include "iinputreceiver.h"
#include "effector.h"

class ENGINE_API CDemoRecord :
	public CEffectorCam,
	public IInputReceiver,
	public pureRender
{
private:
	static struct force_position 
	{
			bool	set_position;
			Fvector p;
	} g_position;

	Fmatrix m_Camera;

	int			iCount;
	IWriter*	file;
	Fvector		m_HPB;
	Fvector		m_Position;
	u32			m_Stage;

	Fvector		m_vT;
    Fvector		m_vR;
	Fvector		m_vVelocity;
	Fvector		m_vAngularVelocity;

	bool		m_bMakeCubeMap;
	bool		m_bMakeScreenshot;
	int			m_iLMScreenshotFragment;
	bool		m_bMakeLevelMap;

	float		m_fSpeed0;
	float		m_fSpeed1;
	float		m_fSpeed2;
	float		m_fSpeed3;
	float		m_fAngSpeed0;
	float		m_fAngSpeed1;
	float		m_fAngSpeed2;
	float		m_fAngSpeed3;

	void		MakeCubeMapFace			(Fvector &D, Fvector &N);
	void		MakeLevelMapProcess		();
	void		MakeScreenshotFace		();
	void		RecordKey				();
	void		MakeCubemap				();
	void		MakeScreenshot			();
	void		MakeLevelMapScreenshot	(bool bHQ);
public:
				CDemoRecord				(const char *name, float life_time=60*60*1000);
	virtual		~CDemoRecord();

	virtual void IR_OnKeyboardPress		(u8 dik);
	virtual void IR_OnKeyboardHold		(u8 dik);
	virtual void IR_OnMouseMove			(int dx, int dy);
	
	virtual BOOL ProcessCam				(SCamEffectorInfo& info);
	static	void SetGlobalPosition		( const Fvector &p ) { g_position.p.set(p), g_position.set_position= true; }
	static	void GetGlobalPosition		( Fvector &p ) { p.set( g_position.p ); }
	bool		 m_b_redirect_input_to_level;
	virtual void OnRender				();
};
