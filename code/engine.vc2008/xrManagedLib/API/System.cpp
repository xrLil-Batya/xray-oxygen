#include "stdafx.h"
#include "System.h"
#include <msclr/marshal_cppstd.h>

void XRay::System::Initialize(String ^ appName)
{
	msclr::interop::marshal_context^ marshal = gcnew msclr::interop::marshal_context();
	const char* appNameC = marshal->marshal_as<const char*>(appName);
	::Core._initialize(appNameC, nullptr, false, "fs.ltx");
}