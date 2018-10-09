#include "stdafx.h"
#include "EObject.h"

using namespace System;

XRay::Editor::EObject^ XRay::Editor::EObject::CreateEObject(XRay::File^ file)
{
	IntPtr pReader = file->GetUnderlyingReader();
	IReader* pRealReader = (IReader*)pReader.ToPointer();

	IReader* OBJ = pRealReader->open_chunk(EOBJ_CHUNK_OBJECT_BODY);
	if (OBJ == nullptr)
		return nullptr;

	CEditableObject* EdObject = new CEditableObject("AWDA");
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
