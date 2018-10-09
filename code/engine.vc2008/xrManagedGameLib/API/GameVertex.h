#pragma once
#include "xrServerEntities/game_graph_space.h"
using namespace System;

namespace XRay
{
	public value class GameVertex
	{
	public:
		GameVertex(IntPtr InNativeObject);

		property ::System::Numerics::Vector3 LevelPoint
		{
			::System::Numerics::Vector3 get()
			{
				return Utils::FromFvector(IntPtr((void*)&pNativeObject->level_point()));
			}
		}

		property ::System::Numerics::Vector3 GamePoint
		{
			::System::Numerics::Vector3 get()
			{
				return Utils::FromFvector(IntPtr((void*)&pNativeObject->game_point()));
			}
		}

		property Byte LevelID
		{
			Byte get()
			{
				return pNativeObject->level_id();
			}
		}

		property UInt32 LevelVertexID
		{
			UInt32 get()
			{
				return pNativeObject->level_vertex_id();
			}
		}

		property array<Byte>^ Mask
		{
			array<Byte>^ get()
			{
				array<Byte>^ Result = gcnew array<::System::Byte>(4);

				const u8* pMask = pNativeObject->vertex_type();
				Result[0] = pMask[0];
				Result[1] = pMask[1];
				Result[2] = pMask[2];
				Result[3] = pMask[3];

				return Result;
			}
		}

		property Byte EdgeCount
		{
			::System::Byte get()
			{
				return pNativeObject->edge_count();
			}
		}

		property UInt32 EdgeOffset
		{
			UInt32 get()
			{
				return pNativeObject->edge_offset();
			}
		}

	private:

		GameGraph::CVertex* pNativeObject;
	};
}