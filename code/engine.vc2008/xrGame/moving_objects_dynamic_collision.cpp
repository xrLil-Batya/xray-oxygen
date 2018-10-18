////////////////////////////////////////////////////////////////////////////
//	Module 		: moving_objects_dynamic_collision.cpp
//	Created 	: 27.03.2007
//  Modified 	: 13.06.2007
//	Author		: Dmitriy Iassenev
//	Description : moving objects with dynamic objects collision, i.e. objects with predictable behaviour
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "moving_objects.h"
#include "ai_space.h"
#include "level_graph.h"
#include "moving_object.h"
#include "moving_objects_impl.h"
#include "ai_obstacle.h"
#include "../../3rd-party/min_obb/magic_box3.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

IC	MagicBox3 &moving_objects::continuous_box	(moving_object *object, const Fvector &position, MagicBox3 &result, const bool &use_box_enlargement) const
{
	result						= object->object().obstacle().min_box();
	object->object().Center		(result.Center());
	result.Center().add			(Fvector().sub(position,object->position()));
	
	if (use_box_enlargement && (object->action() != moving_object::action_move)) {
		result.Extent(0)		*= wait_radius_factor;
		result.Extent(2)		*= wait_radius_factor;
	}

	return						(result);
}

struct collision_predicate {
	const moving_objects::COLLISION				*m_collision;

	IC				collision_predicate			(const moving_objects::COLLISION &collision) :
		m_collision	(&collision)
	{
	}

	IC	bool		operator()					(const moving_objects::COLLISION_TIME &collision_time) const
	{
		const moving_objects::COLLISION	&object = collision_time.second.second;
		if (m_collision->first != object.first)
			return	(false);

		if (m_collision->second != object.second)
			return	(false);

		return		(true);
	}
};

struct boxes {
	MagicBox3	_0;
	MagicBox3	_1;
};

void moving_objects::resolve_collision_first	(boxes &current, moving_object *object0, moving_object *object1, possible_actions &action) const
{
	boxes						start;
	continuous_box				(object0,object0->position(),start._0,false);
	continuous_box				(object1,object1->position(),start._1,false);

	boxes						target;
	continuous_box				(object0,object0->target_position(),target._0,false);
	continuous_box				(object1,object1->target_position(),target._1,false);

	if (target._0.intersects(start._1)) {
		action					= possible_action_1_can_wait_2;
		return;
	}

	if (target._1.intersects(start._0)) {
		action					= possible_action_2_can_wait_1;
		return;
	}

	if (start._0.intersects(current._1)) {
		action					= possible_action_2_can_wait_1;
		return;
	}

	if (start._1.intersects(current._0)) {
		action					= possible_action_1_can_wait_2;
		return;
	}

	if (current._0.intersects(target._1)) {
		action					= possible_action_2_can_wait_1;
		return;
	}

	if (current._1.intersects(target._0)) {
		action					= possible_action_1_can_wait_2;
		return;
	}

	action						= possible_action_1_can_wait_2;
}

void moving_objects::resolve_collision_previous	(boxes &current, moving_object *object0, moving_object *object1, possible_actions &action) const
{
	boxes						start;
	continuous_box				(object0,object0->position(),start._0,true);
	continuous_box				(object1,object1->position(),start._1,true);

	boxes						target;
	continuous_box				(object0,object0->target_position(),target._0,true);
	continuous_box				(object1,object1->target_position(),target._1,true);

	action						= possible_action_1_can_wait_2;
	if (target._0.intersects(start._1)) {
		return;
	}

	if (start._1.intersects(current._0)) {
		return;
	}

	if (current._1.intersects(target._0)) {
		return;
	}

	action						= possible_action_2_can_wait_1;
	if (action == possible_action_2_can_wait_1) {
		if (target._1.intersects(start._0)) {
			return;
		}

		if (start._0.intersects(current._1)) {
			return;
		}

		if (current._0.intersects(target._1)) {
			return;
		}
	}

	action						= possible_action_1_can_wait_2;
}

void moving_objects::resolve_collision			(boxes &current, moving_object *object0, const Fvector &position0, moving_object *object1, const Fvector &position1, possible_actions &action) const
{
	VERIFY_FORMAT(object0->action_frame() != Device.dwFrame, "%d %s",Device.dwFrame,*object0->object().cName());
	VERIFY_FORMAT(object0->action_frame() < Device.dwFrame,  "%d %s",Device.dwFrame,*object0->object().cName());

	VERIFY_FORMAT(object1->action_frame() != Device.dwFrame, "%d %s",Device.dwFrame,*object0->object().cName());
	VERIFY_FORMAT(object1->action_frame() < Device.dwFrame,  "%d %s",Device.dwFrame,*object0->object().cName());

	bool first_time = (std::find_if(m_previous_collisions.begin(), m_previous_collisions.end(),
			collision_predicate(std::make_pair(object0, object1))) == m_previous_collisions.end());

	if (first_time) {
		resolve_collision_first	(current, object0, object1, action);
		return;
	}

	if (object0->action() == moving_object::action_wait) {
		resolve_collision_previous	(current, object0, object1, action);
		return;
	}

	if (object1->action() == moving_object::action_wait) {
		resolve_collision_previous	(current, object1, object0, action); //-V764
		action					= possible_actions(action ^ (possible_action_1_can_wait_2 | possible_action_2_can_wait_1));
		return;
	}

	VERIFY_FORMAT(false, "NODEFAULT: [%s][%s]",*object0->object().cName(),*object1->object().cName());
}

bool moving_objects::collided_dynamic			(moving_object *object0, const Fvector &position0, moving_object *object1, const Fvector &position1, boxes &result) const
{
	continuous_box				(object0,position0,result._0,true);
	continuous_box				(object1,position1,result._1,true);
	return						(result._0.intersects(result._1));
}

bool moving_objects::collided_dynamic(moving_object *object0, const Fvector &position0, moving_object *object1, const Fvector &position1) const
{
	boxes temp;
	return (collided_dynamic(object0, position0, object1, position1, temp));
}

bool moving_objects::collided_dynamic(moving_object *object0, const Fvector &position0, moving_object *object1, const Fvector &position1, possible_actions &action) const
{
#ifdef DEBUG
	for (auto stdPairCollisions : m_collisions)
	{
		VERIFY(!already_wait(object0));
		VERIFY(!already_wait(object1));
	}
#endif // DEBUG


	boxes current;
	if (!collided_dynamic(object0,position0,object1,position1,current))
		return (false);

	resolve_collision(current, object0, position0, object1, position1, action);
	return (true);
}