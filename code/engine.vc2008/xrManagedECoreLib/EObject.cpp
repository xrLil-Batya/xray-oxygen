#include "stdafx.h"
#include "EObject.h"
#include "Include/xrRender/RenderVisual.h"

using namespace System;

XRay::Editor::EObject^ XRay::Editor::EObject::CreateEObject(XRay::File^ file, String^ name)
{
	IReader* pReader = (IReader*)file->NativeReader.ToPointer();

	IReader* OBJ = pReader->open_chunk(EOBJ_CHUNK_OBJECT_BODY);
	if (!OBJ)
	{
		return nullptr;
	}

	string512 asciiName;
	ConvertDotNetStringToAscii(name, asciiName);
	CEditableObject* EdObject = new CEditableObject(asciiName);
	if (!EdObject->Load(*OBJ))
	{
		return nullptr;
	}

	return gcnew EObject(EdObject);
}

XRay::Editor::EObject::~EObject()
{
	delete EditObject;
}

XRay::Editor::EObject::EObject(CEditableObject* InEdObject)
	: EditObject(InEdObject)
{}

XRay::Model^ XRay::Editor::EObject::GetVisualFromEditObject()
{
	CMemoryWriter ogfInMem;
	if (EditObject->PrepareOGF(ogfInMem, 4, true, nullptr))
	{
		CMemoryReader ogfReader(ogfInMem.pointer(), ogfInMem.size());
		IntPtr boxedReader(&ogfReader);
		XRay::File^ file = gcnew XRay::File(boxedReader);
		
		return XRay::Render::LoadModel(file, Name);
	}

	return nullptr;
}
