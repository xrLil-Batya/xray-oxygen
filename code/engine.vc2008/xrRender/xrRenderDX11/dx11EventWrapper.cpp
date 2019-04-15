#include "stdafx.h"
#pragma hdrstop

dxPixEventWrapper::dxPixEventWrapper(LPCWSTR wszName)
{
	if (HW.pAnnotation) HW.pAnnotation->BeginEvent(wszName);
}

dxPixEventWrapper::~dxPixEventWrapper()
{
	if (HW.pAnnotation) HW.pAnnotation->EndEvent();
}