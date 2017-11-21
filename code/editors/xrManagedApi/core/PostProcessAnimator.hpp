#include "../../../engine.vc2008/xrCore/xrCore.h"
#include "../../../engine.vc2008/xrCore/PostprocessAnimator.h"

class CPostprocessAnimator
{
protected:
	CPostProcessParam	*m_Params[POSTPROCESS_PARAMS_COUNT];
	shared_str			m_Name;
	SPPInfo				m_EffectorParams;
	float				m_factor;
	float				m_dest_factor;
	bool				m_bStop;
	float				m_factor_speed;
	bool				m_bCyclic;
	float				m_start_time;
	float				f_length;

	void				Update(float tm);
public:
						CPostprocessAnimator(int id, bool cyclic);
						CPostprocessAnimator();
	virtual				~CPostprocessAnimator();
	void				Clear();
	void				Load(LPCSTR name);
IC  LPCSTR				Name() { return *m_Name; }
	virtual void		Stop(float speed);
	void				SetDesiredFactor(float f, float sp);
	void				SetCurrentFactor(float f);
	void				SetCyclic(bool b) { m_bCyclic = b; }
	float				GetLength();
	virtual	BOOL		Process(float dt, SPPInfo &PPInfo);
	SPPInfo				EfParam() { return m_EffectorParams; }
	void				Create();
	CPostProcessParam*  GetParam(pp_params param);
	void				ResetParam(pp_params param);
	void				Save(LPCSTR name);
};