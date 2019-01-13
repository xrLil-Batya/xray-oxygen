#include "stdafx.h"
#include "xrMemoryDebug.h"

xr_set<void*, std::less<>, PointerAllocator<void*>> gSpecialPointers;
xr_set<void*, std::less<>, PointerAllocator<void*>> gPointerRegistry;
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
		if (gSpecialPointers.find(ptr) != gSpecialPointers.end())
		{
			// The choosen one is gone. We should know about that
			DebugBreak();
			gSpecialPointers.erase(ptr);
		}
        size_t elemErased = gPointerRegistry.erase(ptr);
        gPointerRegistryProtector.Leave();

        R_ASSERT2(elemErased == 1, "Pointer is not registered, heap can be corruped");
    }
}

void MarkPointerAsChoosenOne(void* ptr)
{
	if (gModulesLoaded)
	{
		R_ASSERT2(ptr, "The choosen one must be something!");
		gPointerRegistryProtector.Enter();
		gSpecialPointers.insert(ptr);
		gPointerRegistryProtector.Leave();
	}
}
