#include "stdafx.h"
#include "Model.h"
#include "Include/xrRender/RenderVisual.h"

IRenderVisual* GetPrivateObject(IntPtr ptr)
{
	return reinterpret_cast<IRenderVisual*> (static_cast<void*>(ptr));
}

namespace XRay
{
	String^ Model::Name::get()
	{
		IRenderVisual* privateObj = GetPrivateObject(_nativeObject);
		if (privateObj != nullptr)
		{
			shared_str debugName = privateObj->getDebugName();
			return gcnew String(debugName.c_str(), 0, (int)debugName.size());
		}


		return String::Empty;

	}

	Model::Model(IntPtr InNativeObject)
		: _nativeObject(InNativeObject)
	{}

	Model::~Model()
	{
		IRenderVisual* pNativeVisual = GetPrivateObject(_nativeObject);
		delete pNativeVisual;
	}

}