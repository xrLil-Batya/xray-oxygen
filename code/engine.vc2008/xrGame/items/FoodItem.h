#pragma once
#include "../eatable_item_object.h"

class CFoodItem: public CEatableItemObject
{
	using inherited = CEatableItemObject;
public:
					CFoodItem		();
	virtual			~CFoodItem		();
	
	
	virtual	void	Hit				(SHit* pHDS);
	virtual void	Load			(LPCSTR section);
			void	OnEvent			(NET_Packet& P, u16 type);
			void	BreakToPieces	() noexcept;
protected:
	//партиклы разбивания бутылки
	shared_str m_sBreakParticles;
	ref_sound sndBreaking;

	bool bIsBottele;
	bool bHasBreakSnd;
};
