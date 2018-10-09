#include "stdafx.h"
#include "xrMemoryDebug.h"

std::set<void*, std::less<>, PointerAllocator<void*>> gPointerRegistry;
xrCriticalSection gPointerRegistryProtector;

void RegisterPointer(void* ptr)
{
    if (gModulesLoaded && ptr != nullptr)
    {
        gPointerRegistryProtector.Enter();
        gPointerRegistry.insert(ptr);
        gPointerRegistryProtector.Leave();
    }
}

void UnregisterPointer(void* ptr)
{
    if (gModulesLoaded && ptr != nullptr)
    {
        gPointerRegistryProtector.Enter();
        size_t elemErased = gPointerRegistry.erase(ptr);
        gPointerRegistryProtector.Leave();

        R_ASSERT2(elemErased == 1, "Pointer is not registered, heap can be corruped");
    }
}
