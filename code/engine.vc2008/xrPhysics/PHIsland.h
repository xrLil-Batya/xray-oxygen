#pragma once

#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include "../../3rd-party/ode/src/objects.h"
#include "../../3rd-party/ode/src/joint.h"
#pragma warning(default:4995)
#pragma warning(default:4267)

#include "../../3rd-party/ode/include/ode/objects.h"
#include "PhysicsCommon.h"

class CPHIslandFlags
{
	static const	int base = 8;
	static const	int shift_to_variable = base / 2;
	static const	int mask_static = 0xf;
	Flags8 flags;

	enum
	{
		stActive = 1 << 0,
		flPrefereExactIntegration = 1 << 1
	};
public:

	CPHIslandFlags() { init(); }

	inline void init() { flags.zero(); flags.set(stActive, TRUE); unmerge(); }
	inline BOOL is_active() { return flags.test(stActive << shift_to_variable); }

	inline void set_prefere_exact_integration() { flags.set(flPrefereExactIntegration, TRUE); }
	inline void uset_prefere_exact_integration() { flags.set(flPrefereExactIntegration, FALSE); }

	inline BOOL is_exact_integration_prefeared() { return flags.test(flPrefereExactIntegration << shift_to_variable); }

	inline void merge(CPHIslandFlags& aflags)
	{
		flags.flags |= aflags.flags.flags & mask_static;
		aflags.flags.set(stActive << shift_to_variable, FALSE);
	}
	inline void unmerge()
	{
		flags.flags = ((flags.flags & mask_static) << shift_to_variable) | (flags.flags & mask_static);
	}
};

class CPHIsland : public dxWorld
{
	CPHIslandFlags m_flags;
	dxBody *m_first_body;
	dxJoint *m_first_joint;
	dxJoint **m_joints_tail;
	dxBody **m_bodies_tail;
	CPHIsland *m_self_active;
	int m_nj;
	int m_nb;
	static	const int JOINTS_LIMIT = 1500;
	static	const int BODIES_LIMIT = 500;
public:
	inline	bool IsObjGroun() { return	nb > m_nb; }

	inline	bool IsJointGroun() { return	nj > m_nj; }

	inline	bool CheckSize() { return nj < JOINTS_LIMIT && nb < BODIES_LIMIT; }

	inline	int MaxJoints() { return JOINTS_LIMIT - nj; }

	inline	int MaxJoints(CPHIsland* island) { return MaxJoints() - island->nj; }

	inline	int MaxBodies(CPHIsland* island) { return BODIES_LIMIT - nb - island->nb; }

	inline	bool CanMerge(CPHIsland* island, int& MAX_JOINTS)
	{
		MAX_JOINTS = MaxJoints(island);
		return MAX_JOINTS > 0 && ((nb + island->nb) < BODIES_LIMIT);
	}

	inline	bool IsActive() { return !!m_flags.is_active(); }

	inline	dWorldID DWorld() { return (dWorldID)this; }

	inline	dWorldID DActiveWorld() { return (dWorldID)DActiveIsland(); }

	inline CPHIsland* DActiveIsland()
	{
		GoActive();
		return	m_self_active;
	}

	inline	void GoActive()
	{
		while (!m_self_active->m_flags.is_active())
			m_self_active = m_self_active->m_self_active;
	}

	inline	void Merge(CPHIsland* island)
	{
		CPHIsland* first_island = DActiveIsland();
		CPHIsland* second_island = island->DActiveIsland();

		if (first_island == second_island)
			return;

		*(second_island->m_joints_tail) = first_island->firstjoint;
		first_island->firstjoint = second_island->firstjoint;

		if (0 == first_island->nj && 0 != second_island->nj)
		{
			first_island->m_joints_tail = second_island->m_joints_tail;
		}

		*(second_island->m_bodies_tail) = first_island->firstbody;
		first_island->firstbody = second_island->firstbody;

		first_island->nj += second_island->nj;
		first_island->nb += second_island->nb;
		VERIFY(!(*(first_island->m_bodies_tail)));
		VERIFY(!(*(first_island->m_joints_tail)));
		VERIFY(!((!(first_island->nj)) && (first_island->firstjoint)));
		second_island->m_self_active = first_island;

		m_flags.merge(second_island->m_flags);
	}

	inline	void Unmerge()
	{
		firstjoint = m_first_joint;
		firstbody = m_first_body;
		if (!m_nj)
		{
			m_joints_tail = &firstjoint;
			*m_joints_tail = 0;
		}
		else
			firstjoint->tome = (dObject**)&firstjoint;

		*m_joints_tail = 0;
		*m_bodies_tail = 0;

		m_flags.unmerge();
		m_self_active = this;
		nj = m_nj;
		nb = m_nb;
	}

	inline	void Init()
	{
		m_flags.init();
		m_nj = nj = 0;
		m_nb = nb = 0;
		m_first_joint = firstjoint = 0;
		m_first_body = firstbody = 0;
		m_joints_tail = &firstjoint;
		m_bodies_tail = &firstbody;
		m_self_active = this;
	}

	inline	void AddBody(dxBody* body)
	{
		VERIFY2(m_nj == nj && m_nb == nb && m_flags.is_active(), "can not remove/add during processing phase");
		dWorldAddBody(DWorld(), body);
		m_first_body = body;
		if (m_nb == 0)
		{
			m_bodies_tail = (dxBody**)&body->next;
		}
		m_nb++;
	}

	inline void RemoveBody(dxBody* body)
	{
		VERIFY2(m_nj == nj && m_nb == nb && m_flags.is_active(), "can not remove/add during processing phase");
		if (m_first_body == body)m_first_body = (dxBody*)body->next;
		if (m_bodies_tail == (dxBody**)(&(body->next)))
		{
			m_bodies_tail = (dxBody**)body->tome;
		}
		dWorldRemoveBody((dxWorld*)this, body);
		m_nb--;
	}
	inline	void AddJoint(dxJoint* joint)
	{
		VERIFY2(m_nj == nj && m_nb == nb && m_flags.is_active(), "can not remove/add during processing phase");
		dWorldAddJoint(DWorld(), joint);
		m_first_joint = joint;
		if (!m_nj)
		{
			VERIFY(joint->next == 0);
			m_joints_tail = (dxJoint**)(&(joint->next));
		}
		m_nj++;
	}

	inline void ConnectJoint(dxJoint* joint)
	{
		if (!nj)
		{
			m_joints_tail = (dxJoint**)(&(joint->next));
			VERIFY(!firstjoint);
		}
		dWorldAddJoint(DWorld(), joint);
		VERIFY(!(*(m_joints_tail)));
	}

	inline void DisconnectJoint(dxJoint* joint) { dWorldRemoveJoint(DWorld(), joint); }

	inline void ConnectBody(dxBody* body) { dWorldAddBody(DWorld(), body); }

	inline	void DisconnectBody(dxBody* body) { dWorldRemoveBody(DWorld(), body); }

	inline void RemoveJoint(dxJoint* joint)
	{
		VERIFY2(m_nj == nj && m_nb == nb && m_flags.is_active(), "can not remove/add during processing phase");
		if (m_first_joint == joint)
			m_first_joint = (dxJoint*)joint->next;
		if (m_joints_tail == (dxJoint**)(&(joint->next)))
		{
			m_joints_tail = (dxJoint**)joint->tome;
		}
		dWorldRemoveJoint(DWorld(), joint);
		VERIFY(!*(m_joints_tail));
		m_nj--;
	}
	inline void SetPrefereExactIntegration() { m_flags.set_prefere_exact_integration(); }
	void Step(dReal step);
	void Enable();
	void Repair();
protected:
private:
};