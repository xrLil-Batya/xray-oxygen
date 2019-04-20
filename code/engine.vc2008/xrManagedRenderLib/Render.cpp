#include "stdafx.h"
#include "Render.h"
#include "xrEngine/Render.h"
#include "xrManagedLib/HelperFuncs.h"


XRay::Model^ XRay::Render::LoadModel(XRay::File^ file, String^ name)
{
	IReader* modelReader = (IReader*)static_cast<void*>(file->NativeReader);
	string512 modelName;
	ConvertDotNetStringToAscii(name, modelName);

	IRenderVisual* pVisual = ::Render->model_Create(modelName, modelReader);
	if (pVisual == nullptr)
	{
		return nullptr;
	}
	IntPtr BoxedVisual(pVisual);

	return gcnew XRay::Model(BoxedVisual);
}

