#include "stdafx.h"
#include "FoodItem.h"

inline const float fBreakPower = 5.f;

CFoodItem::CFoodItem() : bIsBottele(false), bHasBreakSnd(false)
{
	m_sBreakParticles = nullptr;
}

CFoodItem::~CFoodItem()
{
	sndBreaking.destroy();
}

void CFoodItem::Load(LPCSTR section)
{
	inherited::Load(section);

	if (pSettings->line_exist(section, "bottle_item") && pSettings->r_bool(section, "bottle_item"))
	{
		if (pSettings->line_exist(section, "break_particles"))
		{
			m_sBreakParticles = pSettings->r_string(section, "break_particles");
		}

		if (bHasBreakSnd = pSettings->line_exist(section, "break_sound"))
		{
			sndBreaking.create(pSettings->r_string(section, "break_sound"), st_Effect, sg_SourceType);
		}

		bIsBottele = true;
	}
}

void CFoodItem::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent(P, type);
	if (bIsBottele)
	{
		switch (type)
		{
		case GE_GRENADE_EXPLODE:
			BreakToPieces();
			break;
		}
	}
}

void CFoodItem::BreakToPieces()
{
	//играем звук
	if(bHasBreakSnd)
		sndBreaking.play_at_pos(0, Position(), false);

	if (bIsBottele)
	{
		//отыграть партиклы разбивания
		if (m_sBreakParticles.size())
		{
			//показываем эффекты
			CParticlesObject* pStaticPG;
			pStaticPG = CParticlesObject::Create(m_sBreakParticles.c_str(), TRUE);
			pStaticPG->play_at_pos(Position());
		}

		//ликвидировать сам объект 
		if (Local())
		{
			DestroyObject();
		}
	}
}

void CFoodItem::Hit(SHit* pHDS)
{
	inherited::Hit(pHDS);

	if (bIsBottele && (pHDS->damage() > fBreakPower))
	{
		//Generate Expode event
		if (Local())
		{
			NET_Packet P;
			u_EventGen(P, GE_GRENADE_EXPLODE, ID());
			u_EventSend(P);
		};
	}
}

