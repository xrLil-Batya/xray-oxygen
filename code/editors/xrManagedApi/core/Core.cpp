#include "Pch.hpp"
#include "Core.hpp"
#include "xrCore/xrCore.h"
#include <msclr/marshal_cppstd.h>

namespace XRay
{
namespace ManagedApi
{
namespace Core
{
static void LogCallbackWrapper(void* context, const char* msg)
{
    if (!Core::ManagedLogCallback)
        return;
    String ^ tmpMsg = msg ? gcnew String(msg) : nullptr;
    Core::ManagedLogCallback(tmpMsg);
}

void Core::Initialize(String ^ appName, LogCallback ^ logCallback, bool initFs, String ^ fsFileName)
{
    ManagedLogCallback = logCallback;
	msclr::interop::marshal_context^ marshal = gcnew msclr::interop::marshal_context();
	const char* appNameC = marshal->marshal_as<const char*>(appName);// msclr::::marshal_as<const char*>(appName);
    if (fsFileName)
    {
        std::string fsFileNameC = msclr::interop::marshal_as<std::string>(fsFileName);
        ::Core._initialize(appNameC, ::LogCallback(LogCallbackWrapper), initFs, fsFileNameC.c_str());
    }
    else
        ::Core._initialize(appNameC);
}

void Core::Initialize(String ^ appName, LogCallback ^ logCallback, bool initFs)
{
    Core::Initialize(appName, logCallback, initFs, nullptr);
}

void Core::Initialize(String ^ appName, LogCallback ^ logCallback)
{
    Core::Initialize(appName, logCallback, false, nullptr);
}

void Core::Initialize(String ^ appName) { Core::Initialize(appName, nullptr, false, nullptr); }
void Core::Destroy() { ::Core._destroy(); }
#pragma todo("FX to FX: Add build-info to xrCore.")
const String ^ Core::GetBuildDate() { return msclr::interop::marshal_as<String^>( /* ::Core.GetBuildDate()); */ ""); }

const UInt32 Core::GetBuildId() { return /* ::Core.GetBuildId(); */ 6732; }

}
}
}
