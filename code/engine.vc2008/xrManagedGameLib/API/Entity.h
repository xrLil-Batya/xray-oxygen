#pragma once
#include "xrGame/Entity.h"
#include "API/PhysicalGameObject.h"

using namespace System;

namespace XRay
{
	public ref class Entity : public PhysicalGameObject
	{
	public:
		Entity(IntPtr InNativeObject);

		property float Health
		{
			float get()
			{
				return pNativeObject->GetfHealth();
			}

			void set(float value)
			{
				pNativeObject->SetfHealth(value);
			}
		}

		property float MaxHealth
		{
			float get()
			{
				return pNativeObject->GetMaxHealth();
			}

			void set(float value)
			{
				pNativeObject->SetMaxHealth(value);
			}
		}

		property bool IsJumping
		{
			bool get()
			{
				CEntity::SEntityState EntityState;
				if (pNativeObject->g_State(EntityState))
				{
					return EntityState.bJump;
				}
				return false;
			}
		}

		property bool IsFalling
		{
			bool get()
			{
				CEntity::SEntityState EntityState;
				if (pNativeObject->g_State(EntityState))
				{
					return EntityState.bFall;
				}
				return false;
			}
		}

		property bool IsCrouching
		{
			bool get()
			{
				CEntity::SEntityState EntityState;
				if (pNativeObject->g_State(EntityState))
				{
					return EntityState.bCrouch;
				}
				return false;
			}
		}

		property bool IsSprinting
		{
			bool get()
			{
				CEntity::SEntityState EntityState;
				if (pNativeObject->g_State(EntityState))
				{
					return EntityState.bSprint;
				}
				return false;
			}
		}

		property bool IsAlive
		{
			bool get()
			{
				return pNativeObject->g_Alive();
			}
		}

		property Int32 Team
		{
			Int32 get()
			{
				return pNativeObject->g_Team();
			}

			void set(Int32 value)
			{
				pNativeObject->ChangeTeam(value, pNativeObject->g_Squad(), pNativeObject->g_Group());
			}
		}

		property Int32 Squad
		{
			Int32 get()
			{
				return pNativeObject->g_Squad();
			}

			void set(Int32 value)
			{
				pNativeObject->ChangeTeam(pNativeObject->g_Team(), value, pNativeObject->g_Group());
			}
		}

		property Int32 Group
		{
			Int32 get()
			{
				return pNativeObject->g_Group();
			}

			void set(Int32 value)
			{
				pNativeObject->ChangeTeam(pNativeObject->g_Team(), pNativeObject->g_Squad(), value);
			}
		}

		property float Morale
		{
			float get()
			{
				return pNativeObject->m_fMorale;
			}
			
			void set(float value)
			{
				pNativeObject->m_fMorale = value;
			}
		}

		// Will be refactored later
		void ChangeTeam(int team, int squad, int group);



	private:

		CEntity* pNativeObject;
	};
}