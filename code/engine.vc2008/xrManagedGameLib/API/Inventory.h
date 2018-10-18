#pragma once
#include "xrGame/InventoryOwner.h"
#include "xrGame/Inventory.h"
#include "API/PDA.h"

using namespace System;

namespace XRay
{
	public ref class Inventory : public NativeObject
	{
	public:
		Inventory(IntPtr InNativeObject);

		property float TotalWeight
		{
			float get()
			{
				return pNativeObject->inventory().CalcTotalWeight();
			}
		}

		// Can return null, be careful
		property PDA^ pda
		{
			PDA^ get()
			{
				if (CPda* pPDA = pNativeObject->GetPDA())
				{
					return gcnew PDA(IntPtr(pPDA));
				}

				return nullptr;
			}
		}

		property UInt32 money
		{
			UInt32 get()
			{
				return pNativeObject->get_money();
			}

			void set(UInt32 value)
			{
				pNativeObject->set_money(value, true);
			}
		}

		property String^ name
		{
			String^ get()
			{
				return gcnew String (pNativeObject->Name());
			}

			void set(String^ value)
			{
				array<unsigned char>^ asciiName = ::System::Text::Encoding::ASCII->GetBytes(value);
				pin_ptr<unsigned char> bufferPtr = &asciiName[0];
				pNativeObject->set_name((LPCSTR)*bufferPtr);
			}
		}

		property bool CanTalk
		{
			bool get()
			{
				return pNativeObject->IsTalkEnabled();
			}

			void set(bool value)
			{
				if (value)
				{
					pNativeObject->EnableTalk();
				}
				else
				{
					pNativeObject->DisableTalk();
				}
			}
		}

		property bool CanTrade
		{
			bool get()
			{
				return pNativeObject->IsTradeEnabled();
			}

			void set(bool value)
			{
				if (value)
				{
					pNativeObject->EnableTrade();
				}
				else
				{
					pNativeObject->DisableTrade();
				}
			}
		}

	private:

		CInventoryOwner* pNativeObject;
	};
}