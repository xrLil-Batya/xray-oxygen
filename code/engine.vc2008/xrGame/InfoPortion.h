#pragma once
class CInfoPortion
{
	shared_str			m_InfoId;
public:
	void				Load	(const shared_str& info_id) {m_InfoId = info_id;}
	const shared_str&	InfoId	() const					{return m_InfoId;}
};