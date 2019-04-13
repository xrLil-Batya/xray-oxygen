#include "stdafx.h"
#pragma hdrstop

dxPixEventWrapper::dxPixEventWrapper(LPCWSTR wszName)
{
	if (HW.Annotation) HW.Annotation->BeginEvent(wszName);
}

dxPixEventWrapper::~dxPixEventWrapper()
{
	if (HW.Annotation) HW.Annotation->EndEvent();
}