#include "stdafx.h"
#include "EditorOcclusion.h"
#include "../xrEngine/Render.h"
#include "../xrEngine/xr_object.h"


class CustomOcclusion : public ICustomOcclusion
{

public:
	virtual void getRenderableList(IRender_interface* pGraph) override
	{
		XRay::Editor::EditorOcclusion::Instance()->FillOcclusionList(IntPtr(pGraph));
	}
};


XRay::Editor::EditorOcclusion::EditorOcclusion()
{
	_Instance = this;
}

XRay::Editor::EditorOcclusion::~EditorOcclusion()
{
	_Instance = nullptr;
}

void XRay::Editor::EditorOcclusion::AddObjectToRenderableList(XRay::Model^ object)
{
	_renderableList->Add(object);
}

void XRay::Editor::EditorOcclusion::FillOcclusionList(IntPtr pRenderInterface)
{
	IRender_interface* pRenderInterfaceNative =(IRender_interface*) pRenderInterface.ToPointer();

	for each (XRay::Model^ renderObj in _renderableList)
	{
		IRenderVisual* pObj = (IRenderVisual*)renderObj->NativeObject.ToPointer();

		//pRenderInterfaceNative->set_Object(pObj);
		//pObj->renderable_Render();
		//pRenderInterfaceNative->set_Object(nullptr);
		pRenderInterfaceNative->add_Visual(pObj);
	}
}

XRay::Editor::EditorOcclusion^ XRay::Editor::EditorOcclusion::Instance()
{
	return _Instance;
}

