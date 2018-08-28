#pragma once
#include "../xrCore/PostprocessAnimator.h"
#include "ActorEffector.h"

class CPostprocessAnimator : public CEffectorPP, public BasicPostProcessAnimator
{
public:
	CPostprocessAnimator(int id, bool cyclic) : CEffectorPP((EEffectorPPType)id, 100000, true), BasicPostProcessAnimator(id, cyclic) {}
	CPostprocessAnimator() = default;
	virtual void Stop(float speed) override;
	virtual void Load(LPCSTR name, bool internalFs = true);
	virtual BOOL Valid();
	virtual BOOL Process(SPPInfo& PPInfo);
};

#include <functional>
class CPostprocessAnimatorLerp : public CPostprocessAnimator
{
protected:
	std::function<float()> m_get_factor_func;

public:
	void SetFactorFunc(std::function<float()> f) { m_get_factor_func = f; }
	virtual BOOL Process(SPPInfo& PPInfo);
};

class CPostprocessAnimatorLerpConst : public CPostprocessAnimator
{
protected:
	float m_power;

public:
	CPostprocessAnimatorLerpConst() { m_power = 1.0f; }
	void SetPower(float val) { m_power = val; }
	virtual BOOL Process(SPPInfo& PPInfo);
};

class CPostprocessAnimatorControlled : public CPostprocessAnimatorLerp
{
	CEffectorController* m_controller;

public:
	virtual ~CPostprocessAnimatorControlled();
	CPostprocessAnimatorControlled(CEffectorController* c);
	virtual BOOL Valid();
};