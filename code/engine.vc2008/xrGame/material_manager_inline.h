////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC size_t CMaterialManager::last_material_idx		() const
{
	return				(m_last_material_idx);
}

IC size_t CMaterialManager::self_material_idx		() const
{
	return				(m_my_material_idx);
}

IC SGameMtlPair *CMaterialManager::get_current_pair()
{
	m_movement_control->update_last_material	();
	return GMLib.GetMaterialPair(m_my_material_idx,m_last_material_idx);
}
