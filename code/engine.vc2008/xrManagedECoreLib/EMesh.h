#pragma once
#include "EditMesh.h"

using namespace System;
using namespace System::Numerics;

namespace XRay::Editor
{
	public ref class EMesh
	{
	public:
		EMesh(CEditableMesh* InEditableMesh);

		property array<Vector3>^ Vertexes
		{
			array<Vector3>^ get()
			{
				array<Vector3>^ Result = gcnew array<Vector3>(EditableMesh->GetVCount());

				Fvector* pNativeVerts = const_cast<Fvector*> (EditableMesh->GetVertices());

				for (DWORD i = 0; i < EditableMesh->GetVCount(); ++i)
				{
					Result[i] = XRay::Utils::FromFvector(IntPtr(&pNativeVerts[i]));
				}

				return Result;
			}
		}

	private:

		CEditableMesh* EditableMesh;

	};
}