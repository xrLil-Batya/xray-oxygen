#pragma once

namespace 
{ 
		extern "C" void * _ReturnAddress(void);
		DWORD_PTR GetInstructionPtr()
		{
				return (DWORD_PTR)_ReturnAddress();
		}
}

struct StackTraceInfo
{
	static const size_t Capacity = 100;
	static const size_t LineCapacity = 256;
	char Frames[Capacity * LineCapacity];
	size_t Count;

	char* operator[](size_t i) { return Frames + i * LineCapacity; }
};


size_t BuildStackTrace(EXCEPTION_POINTERS* exPtrs, char* buffer, size_t capacity, size_t lineCapacity)
{
	memset(buffer, 0, capacity*lineCapacity);
	auto flags = GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
	auto traceDump = GetFirstStackTraceString(flags, exPtrs);
	int frameCount = 0;
	while (traceDump)
	{
		lstrcpy(buffer + frameCount * lineCapacity, traceDump);
		frameCount++;
		traceDump = GetNextStackTraceString(flags, exPtrs);
	}
	return frameCount;
}
#pragma warning(disable: 4311 4302)
size_t BuildStackTrace(char* buffer, size_t capacity, size_t lineCapacity)
{
	// XXX: add support for x86_64
	// FX: done
	CONTEXT context;
	EXCEPTION_POINTERS ex_ptrs;
	void* ebp;
	context.ContextFlags = CONTEXT_FULL;
	if (GetThreadContext(GetCurrentThread(), &context))
	{
		context.Rip = GetInstructionPtr();
		context.Rbp = (DWORD)&ebp;
		context.Rsp = (DWORD)&context;

		ex_ptrs.ContextRecord = &context;
		ex_ptrs.ExceptionRecord = 0;
		return BuildStackTrace(&ex_ptrs, buffer, capacity, lineCapacity);
	}
	return 0;
}
#pragma warning(default: 4311 4302)

StackTraceInfo StackTrace = {};