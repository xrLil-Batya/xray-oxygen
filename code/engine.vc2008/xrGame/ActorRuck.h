#pragma once
#include "inventory_item_object.h"

class CActorRuck : public CInventoryItemObject 
{
private:
	using inherited = CInventoryItemObject;
	float m_additional_weight;

public:
	CActorRuck() :m_additional_weight(0) {};
	virtual ~CActorRuck() = default;

	virtual void Load(LPCSTR section);
	float AdditionalInventoryWeight() const { return m_additional_weight; };
};