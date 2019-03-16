#include "stdafx.h"
#include "phsimplecalls.h"
#include "../xrphysics/physicsshell.h"
#include "../xrphysics/IPHWorld.h"

CPHCallOnStepCondition::CPHCallOnStepCondition()
{
	if(physics_world())
		set_step(physics_world()->StepsNum());
	else		
		set_step(0);
}

IC bool CPHCallOnStepCondition::time_out() const
{
	return physics_world()->StepsNum()>m_step;
}

bool	CPHCallOnStepCondition::is_true()
{
	return time_out();
}
bool	CPHCallOnStepCondition::obsolete() const
{
	return time_out();
}

void CPHCallOnStepCondition::set_steps_interval(u64 steps)
{
	set_step(physics_world()->StepsNum()+steps);
}
void CPHCallOnStepCondition::set_time_interval(float time)
{
		set_steps_interval(iCeil(time/fixed_step));
}
void CPHCallOnStepCondition::set_time_interval(u32 time)
{
	set_time_interval(float(time)/1000.f);
}
void CPHCallOnStepCondition::set_global_time(float time)
{
	float time_interval=Device.fTimeGlobal-time;
	if(time_interval<0.f)set_step(physics_world()->StepsNum());
	set_time_interval(time_interval);
}
void CPHCallOnStepCondition::set_global_time(u32 time)
{
	set_global_time(float(time)/1000.f);
}

CPHShellBasedAction::CPHShellBasedAction(IPhysicsShellEx	*shell)
{
	VERIFY(shell&&shell->isActive());
	m_shell=shell;
}
bool CPHShellBasedAction::obsolete() const
{
	return !m_shell||!m_shell->isActive();
}

CPHConstForceAction::CPHConstForceAction(IPhysicsShellEx	*shell, const Fvector &force)
:CPHShellBasedAction(shell)
{
	m_force.set(force);
}


void CPHConstForceAction::run()
{
	m_shell->applyForce(m_force.x,m_force.y,m_force.z);
}

CPHReqComparerHasShell::CPHReqComparerHasShell(IPhysicsShellEx	*shell)
{
	VERIFY(shell);
	m_shell=shell;
}


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("gyts",on)
void CPHCallOnStepCondition::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPHCallOnStepCondition>("phcondition_callonstep")
			.def("set_step",				&CPHCallOnStepCondition::set_step)
			.def("set_steps_interval",		&CPHCallOnStepCondition::set_steps_interval)
			.def("set_global_time_ms",		(void(CPHCallOnStepCondition::*)(u32))(&CPHCallOnStepCondition::set_global_time))
			.def("set_global_time_s",		(void(CPHCallOnStepCondition::*)(float))(&CPHCallOnStepCondition::set_global_time))
			.def("set_time_interval_ms",	(void(CPHCallOnStepCondition::*)(u32))(&CPHCallOnStepCondition::set_time_interval))
			.def("set_time_interval_s",		(void(CPHCallOnStepCondition::*)(float))(&CPHCallOnStepCondition::set_time_interval))
			.def(constructor<>())
		];
}

void CPHExpireOnStepCondition::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPHExpireOnStepCondition,CPHCallOnStepCondition>("phcondition_expireonstep")
			.def(constructor<>())
		];
}

void CPHConstForceAction::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPHConstForceAction>("phaction_constforce")
			.def(constructor<IPhysicsShellEx*,const Fvector&>())
		];
}