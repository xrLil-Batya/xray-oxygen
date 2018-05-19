#pragma once
#include "files_list.hpp"

/*
* Base SDK object implementation
*/
class ECORE_API IObject
{

public:

	virtual void load(IReader* reader) = 0;
};