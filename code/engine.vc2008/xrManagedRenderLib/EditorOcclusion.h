#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace XRay
{
	namespace Editor
	{
		public ref class EditorOcclusion
		{
		public:
			EditorOcclusion();
			~EditorOcclusion();
			void AddObjectToRenderableList(XRay::Object^ object);
			void FillOcclusionList(IntPtr pRenderInterface);
			static EditorOcclusion^ Instance();
		private:

			List<XRay::Object^>^ _renderableList;
			IntPtr _opaqueOcclusionInterface;
			static XRay::Editor::EditorOcclusion^ _Instance;
		};
	}
}