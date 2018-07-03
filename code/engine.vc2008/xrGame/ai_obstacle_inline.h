////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_obstacle_inline.h
//	Created 	: 02.04.2007
//  Modified 	: 06.04.2007
//	Author		: Dmitriy Iassenev
//	Description : ai obstacle class inline functions
////////////////////////////////////////////////////////////////////////////
#pragma once
IC	ai_obstacle::ai_obstacle(CGameObject *object)
{
	m_object = object;
	m_actual = false;

	m_min_box = MagicBox3(Fidentity,Fvector().set(.5,1.f,.5f));
	m_min_box.Center().y++;
}

IC	void ai_obstacle::compute() noexcept
{
	if (m_actual)
		return;

	m_actual = true;
	compute_impl();
}

IC	const ai_obstacle::AREA	&ai_obstacle::area()
{
	compute();
	return (m_area);
}

IC	const u32 &ai_obstacle::crc() noexcept
{
	compute();
	return (m_crc);
}