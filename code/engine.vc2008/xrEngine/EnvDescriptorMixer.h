#pragma once

class ENGINE_API CEnvironment;
class ENGINE_API CEnvModifier;

#include "EnvDescriptor.h"

class ENGINE_API CEnvDescriptorMixer : public CEnvDescriptor
{
public:
	FactoryPtr<IEnvDescriptorMixerRender> m_pDescriptorMixer;
	float				weight;				

	float				fog_near;		
	float				fog_far;		
public:
						CEnvDescriptorMixer	(shared_str const& identifier);
	INGAME_EDITOR_VIRTUAL void Lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
	void				Clear				();
	void				Destroy				();
};