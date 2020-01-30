#include "stdafx.h"
#include "xrsharedmem.h"
#include "FrayBuildConfig.hpp"
#include "mimalloc/mimalloc.h"
#include "tbb/tbb_stddef.h"
#include "xrDebugSymbol.h"
#include <filesystem>

xrMemory Memory;

#ifdef TBB_ALLOC
#define USED_TBB_ALLOC 1
#else
#define USED_TBB_ALLOC 0
#endif


void Oxygen_mi_output_fun(const char* msg)
{
	if (xrLogger::IsLogThreadWorking())
	{
		Msg("[Memory]: %s", msg);
	}
	else
	{
		OutputDebugString("[Memory]: ");
		OutputDebugString(msg);
		OutputDebugString("\r\n");
	}

	//#DELETE_ME: !
	static int DebugShit = 0;
	static void* TargetPtr = nullptr;
	if (DebugShit == 1)
	{
		g_pStringContainer.verify();
		Memory.PrintAllPointerHistory(TargetPtr);

		char* tmpPtr = (char*)TargetPtr;
		tmpPtr -= 16;
		Memory.PrintAllPointerHistory(tmpPtr);
	}
}

xrMemory::xrMemory()
{
	// do not place code here. 
	// If you need something initialized - place that in _initialize
}

xrMemory::~xrMemory()
{}

void xrMemory::_initialize()
{
	if constexpr (MEM_PURE_ALLOC)
	{
		hHeap = HeapCreate(0, 0, 0);
	}
	else if constexpr (MEM_MIMALLOC)
	{
		mi_register_output(Oxygen_mi_output_fun);
	}

	if constexpr (MEM_SUPER_HARD_DEBUG)
	{
		initializeHardDebug();
	}

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	dwPageSize = info.dwPageSize;
	bInitialized = true;
}

void xrMemory::_destroy()
{
	g_pSharedMemoryContainer_isDestroyed = true;
	xr_delete(g_pSharedMemoryContainer);
	g_pSharedMemoryContainer = nullptr;
	if constexpr (MEM_PURE_ALLOC)
	{
		HeapDestroy(hHeap);
	}
	bInitialized = false;
}

char TempOut[4096 * 3];

inline const size_t external_size = size_t(-1);

u32 xrMemory::mem_usage()
{
	if constexpr (MEM_PURE_ALLOC || USED_TBB_ALLOC) // Intel TBB do not have implemented own function to get overall size
	{
		_HEAPINFO hinfo;
		int heapstatus;
		hinfo._pentry = nullptr;
		size_t	total = 0;
		while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
		{
			if (hinfo._useflag == _USEDENTRY)
			{
				total += hinfo._size;
			}
		}

		switch (heapstatus)
		{
		case _HEAPEMPTY:
			break;
		case _HEAPEND:
			break;
		case _HEAPBADPTR:
			Msg("[xrMemory::mem_usage]: bad pointer to heap");
			break;
		case _HEAPBADBEGIN:
			Msg("[xrMemory::mem_usage]: bad start of heap");
			break;
		case _HEAPBADNODE:
			Msg("[xrMemory::mem_usage]: bad node in heap");
			break;
		}

		return total;
	}
	else if constexpr (MEM_MIMALLOC)
	{
		auto GetMiMallocInfoLambda = [](const char* msg)
		{
			xr_strconcat(TempOut, TempOut, msg);
		};
		mi_stats_print(GetMiMallocInfoLambda);

		Msg("MiMalloc: \r\n%s", TempOut);
		ZeroMemory(&TempOut[0], sizeof(TempOut));

		return 0;
	}
}

void xrMemory::mem_compact()
{
	g_pStringContainer.clean();

	if (g_pSharedMemoryContainer)
		g_pSharedMemoryContainer->clean();

	if (strstr(Core.Params, "-swap_on_compact"))
		SetProcessWorkingSetSize(GetCurrentProcess(), external_size, external_size);

	if constexpr (MEM_PURE_ALLOC)
	{
		_heapmin();

		HeapCompact(GetProcessHeap(), 0);
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		// Nothing. But do not remove this line
	}
	else if constexpr (MEM_MIMALLOC)
	{
		mi_collect(false);
	}
}

void xrMemory::PrintVersion()
{
	if constexpr (MEM_PURE_ALLOC)
	{
		Msg("Memory library: System");
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		Msg("Memory library: Intel TBB (%d.%d)", TBB_VERSION_MAJOR, TBB_VERSION_MINOR);
	}
	else if constexpr(MEM_MIMALLOC)
	{
		Msg("Memory library: MiMalloc (%d.%d.%d)", MI_MALLOC_VERSION / 100, 
												  (MI_MALLOC_VERSION % 100) / 10, 
												  (MI_MALLOC_VERSION % 100) % 10);
	}
}


void xrMemory::PrintStat()
{
	if constexpr (MEM_PURE_ALLOC || USED_TBB_ALLOC) // Intel TBB do not have implemented own function to get overall size
	{
		_HEAPINFO hinfo;
		int heapstatus;
		hinfo._pentry = nullptr;
		size_t	total = 0;
		u32	blocks_free = 0;
		u32	blocks_used = 0;
		while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
		{
			if (hinfo._useflag == _USEDENTRY)
			{
				total += hinfo._size;
				blocks_used += 1;
			}
			else
				blocks_free += 1;
		}


		switch (heapstatus)
		{
		case _HEAPEMPTY:
			break;
		case _HEAPEND:
			break;
		case _HEAPBADPTR:
			Msg("[xrMemory::PrintStat]: bad pointer to heap");
			break;
		case _HEAPBADBEGIN:
			Msg("[xrMemory::PrintStat]: bad start of heap");
			break;
		case _HEAPBADNODE:
			Msg("[xrMemory::PrintStat]: bad node in heap");
			break;
		}

		Msg("[xrMemory::PrintStat]: Total: %zu, blocks free: %u, blocks used: %u", total, blocks_free, blocks_used);
	}
	else if constexpr (MEM_MIMALLOC)
	{
		auto GetMiMallocInfoLambda = [](const char* msg)
		{
			xr_strconcat(TempOut, TempOut, msg);
		};
		mi_stats_print(GetMiMallocInfoLambda);

		Msg("MiMalloc: \r\n%s", TempOut);
		ZeroMemory(&TempOut[0], sizeof(TempOut));
	}
}

void xrMemory::PrintAllPointerHistory(void* ptr)
{
	FlushAllocEvents();

	ptrdiff_t targetPtr = (ptrdiff_t)ptr;

	// Can't use Msg here
	//Msg("Report all events related to 0x%p", ptr);
	string512 MsgBuffer;
	xr_sprintf(MsgBuffer, "Report all events related to 0x%p\r\n", ptr);
	OutputDebugString(MsgBuffer);

	// reopen file, but this time - for read
	fclose(hAllocHistoryFile);
	hAllocHistoryFile = fopen("AllocationHistory.bin", "rb");
	fseek(hAllocHistoryFile, 0, SEEK_END);

	long fileSize = ftell(hAllocHistoryFile);
	fseek(hAllocHistoryFile, 0, SEEK_SET);

	int recordsAvailable = fileSize / sizeof(AllocRecord);
	// for debug
	size_t allocEvents = 0;
	size_t freedEvents = 0;
	size_t reallocEvents = 0;

	void* lastAllocAddress = nullptr;
	for (int i = 0; i < recordsAvailable; i++)
	{
		AllocRecord record;
		size_t ElemReaded = fread(&record, sizeof(AllocRecord), 1, hAllocHistoryFile);

		if (ElemReaded != 1)
		{
			DebugBreak();
		}

		const char* strAction = "undefined";
		switch (record.AllocFlag)
		{
		case AllocRecordStatus::Allocated:
			strAction = "Allocated";
			++allocEvents;
			break;
		case AllocRecordStatus::Freed:
			strAction = "Freed";
			++freedEvents;
			break;
		case AllocRecordStatus::Realloc:
			strAction = "Realloc";
			++reallocEvents;
			break;
		default:
			break;
		}

		// now, analyze the record
		ptrdiff_t startPtr = (ptrdiff_t)record.ptr;
		ptrdiff_t endPtr = startPtr + (ptrdiff_t)record.size;

		if ((lastAllocAddress == record.ptr) || (targetPtr >= startPtr && targetPtr <= endPtr))
		{
			string1024 SymbolInfo;
			DebugSymbols.ResolveFrame(record.frame, SymbolInfo);
			Time allocTime(record.time);
			string256 TimeOfDay;
			xr_sprintf(TimeOfDay, "[%s.%s.%s] ",
				allocTime.GetHoursString().c_str(), allocTime.GetMinutesString().c_str(), allocTime.GetSecondsString().c_str());

			xr_sprintf(MsgBuffer, "\t%d (tID: %u) %s : 0x%p (size: %zu) was %s in %s\r\n", i, record.threadID, TimeOfDay, record.ptr, record.size, strAction, SymbolInfo);
			OutputDebugString(MsgBuffer);

			if (record.AllocFlag == AllocRecordStatus::Allocated)
			{
				lastAllocAddress = record.ptr;
			}
			else if (record.AllocFlag == AllocRecordStatus::Freed)
			{
				lastAllocAddress = nullptr;
			}
		}
	}

	xr_sprintf(MsgBuffer, "Total stats: alloc events: %zu, freed events: %zu, realloc events: %zu\r\n", allocEvents, freedEvents, reallocEvents);
	OutputDebugString(MsgBuffer);
}


void xrMemory::initializeHardDebug()
{
	hDebugHeap = HeapCreate(0, 0, 0);
	R_ASSERT2(hDebugHeap, "Failed to initialize Debug Heap");

	//std::filesystem::remove("AllocationHistory.bin");
	AllocationHistory = (char*)HeapAlloc(hDebugHeap, 0, AllocationHistorySize);
	hAllocHistoryFile = fopen("AllocationHistory.bin", "wb");
	InitializeCriticalSectionAndSpinCount(&allocEventGuard, 250);
}

void xrMemory::WriteAllocation(void* Ptr, size_t size)
{
	PrepareAllocEvent(Ptr, size, AllocRecordStatus::Allocated);
}

void xrMemory::WriteReAllocation(void* Ptr, size_t size)
{
	PrepareAllocEvent(Ptr, size, AllocRecordStatus::Realloc);
}

void xrMemory::PrepareAllocEvent(void* Ptr, size_t size, AllocRecordStatus status)
{
	if (!DebugSymbols.IsInitialized())
	{
		DebugSymbols.Initialize(true);
	}

	void* Callstack[3];
	ZeroMemory(&Callstack[0], sizeof(Callstack));
	u16 CapturedFrames = DebugSymbols.GetCurrentStack(&Callstack[0], sizeof(Callstack) / sizeof(Callstack[0]));

	if (CapturedFrames != 3)
	{
		DebugBreak();
	}

	AllocRecord record;
	record.ptr = Ptr;
	record.size = size;
	record.frame = Callstack[2];
	record.AllocFlag = status;
	record.time = time(nullptr);
	record.threadID = GetCurrentThreadId();

	WriteAllocRecord(record);
}

void xrMemory::WriteFree(void* Ptr)
{
	PrepareAllocEvent(Ptr, 0, AllocRecordStatus::Freed);
}

void xrMemory::WriteAllocRecord(AllocRecord& record)
{
	EnterCriticalSection(&allocEventGuard);
	// do we have place for a record in mem?
	size_t RemainPlace = AllocationHistorySize - AllocationHistoryWriteCursor;
	if (RemainPlace < sizeof(AllocRecord))
	{
		FlushAllocEvents();
	}

	memcpy(&AllocationHistory[AllocationHistoryWriteCursor], &record, sizeof(AllocRecord));
	AllocationHistoryWriteCursor += sizeof(AllocRecord);
	LeaveCriticalSection(&allocEventGuard);
}

void xrMemory::FlushAllocEvents()
{
	if (AllocationHistoryWriteCursor > 0)
	{
		size_t elems = fwrite(&AllocationHistory[0], AllocationHistoryWriteCursor, 1, hAllocHistoryFile);
		if (elems != 1)
		{
			DebugBreak();
		}
		fflush(hAllocHistoryFile);

		AllocationHistoryWriteCursor = 0;
	}
}

// xr_strdup
char* xr_strdup(const char* string)
{
	if constexpr (MEM_PURE_ALLOC)
	{
		VERIFY(string);
		size_t len = xr_strlen(string) + 1;
		char *	memory = (char*)Memory.mem_alloc(len);
		std::memcpy(memory, string, len);

		return memory;
	}
	else if constexpr (MEM_MIMALLOC)
	{
		return mi_strdup(string);
	}
}

XRCORE_API bool is_stack_ptr(void* _ptr)
{
	int local_value = 0;
	void* ptr_refsound = _ptr;
	void* ptr_local = &local_value;
	ptrdiff_t difference = (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	return (difference < (512 * 1024));
}

extern "C"
{
    XRCORE_API void* xr_malloc_C(size_t size)
    {
        return Memory.mem_alloc(size);
    }

    XRCORE_API void	xr_free_C(void* ptr)
    {
        Memory.mem_free(ptr);
    }

    XRCORE_API void* xr_realloc_C(void* ptr, size_t size)
    {
        return Memory.mem_realloc(ptr, size);
    }
}