#include "stdafx.h"
#include "EObject.h"

using namespace System;

XRay::Editor::EObject^ XRay::Editor::EObject::CreateEObject(XRay::File^ file)
{
	IReader* pReader = (IReader*)file->NativeReader.ToPointer();

	IReader* OBJ = pReader->open_chunk(EOBJ_CHUNK_OBJECT_BODY);
	if (!OBJ)
	{
		return nullptr;
	}

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
