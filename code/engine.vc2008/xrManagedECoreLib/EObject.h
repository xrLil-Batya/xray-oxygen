#pragma once
#include "EditObject.h"
#include "EditMesh.h"
#include "EMesh.h"

using namespace System;

namespace XRay::Editor
{
	public ref class EObject
	{
	public:
		static EObject^ CreateEObject(XRay::File^ file, String^ name);

		virtual ~EObject();

		property String^ Creator
		{
			String^ get()
			{
				shared_str creatorName = EditObject->GetCreatorName();
				if (!creatorName)
				{
					return String::Empty;
				}
				
				return gcnew String(creatorName.data(), 0, creatorName.size());
			}
		}

		property String^ Modificator
		{
			String^ get()
			{
				shared_str modificatorName = EditObject->GetModificatorName();
				if (!modificatorName)
				{
					return String::Empty;
				}

				return gcnew String(modificatorName.data(), 0, modificatorName.size());
			}
		}

		property ::System::DateTime CreationTime
		{
			::System::DateTime get()
			{
				__time32_t creationTime = EditObject->GetCreationTime();
				tm* timeDesc = _gmtime32(&creationTime);
				
				return DateTime(
					timeDesc->tm_year + 1900, timeDesc->tm_mon + 1, timeDesc->tm_mday, // year, month, day
					timeDesc->tm_hour, timeDesc->tm_min, timeDesc->tm_sec);			   // hour, minute, seconds
			}
		}

		property ::System::DateTime ModificationTime
		{
			::System::DateTime get()
			{
				__time32_t creationTime = EditObject->GetModificationTime();
				tm* timeDesc = _gmtime32(&creationTime);

				return DateTime(
					timeDesc->tm_year + 1900, timeDesc->tm_mon + 1, timeDesc->tm_mday, // year, month, day
					timeDesc->tm_hour, timeDesc->tm_min, timeDesc->tm_sec);			   // hour, minute, seconds
			}
		}

		property String^ ClassName
		{
			String^ get()
			{
				xr_string className = EditObject->GetClassScript();
				return gcnew String(className.data(), 0, className.size());
			}
		}

		property String^ Name
		{
			String^ get()
			{
				return gcnew String(EditObject->m_LoadName.data(), 0, EditObject->m_LoadName.size());
			}
		}

		property array<XRay::Editor::EMesh^>^ Meshes
		{
			array<XRay::Editor::EMesh^>^ get()
			{
				int MeshCount = EditObject->MeshCount();
				array<XRay::Editor::EMesh^>^ Result = gcnew array<XRay::Editor::EMesh ^>(MeshCount);

				for (int i = 0; i < MeshCount; ++i)
				{
					Result[i] = gcnew XRay::Editor::EMesh(EditObject->Meshes()[i]);
				}

				return Result;
			}
		}

		property XRay::Model^ renderable
		{
			XRay::Model^ get()
			{
				if (ObjectVisual == nullptr)
				{
					ObjectVisual = GetVisualFromEditObject();
				}

				return ObjectVisual;
			}
		}


	private:
		XRay::Model^ GetVisualFromEditObject();

		EObject(CEditableObject* InEdObject);

		CEditableObject* EditObject;

		///
		/// IRenderVisual*
		///
		XRay::Model^	ObjectVisual;
	};
}