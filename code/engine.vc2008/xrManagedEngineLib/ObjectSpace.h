#pragma once

class CLevel;
class CObjectSpace;
using namespace System;

public ref class ObjectSpace
{
public:
	ObjectSpace();
	ObjectSpace(IntPtr InNativeObject);


private:
	CObjectSpace* pNativeObject;
};

