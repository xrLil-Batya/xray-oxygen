#pragma once

namespace { 
	void __declspec(naked, noinline) * __cdecl GetInstructionPtr()
	{
#ifdef _WIN64
		_asm mov rax, [rsp] _asm retn
#else
		_asm mov eax, [esp] _asm retn
#endif
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

size_t BuildStackTrace(char* buffer, size_t capacity, size_t lineCapacity)
{
	// XXX: add support for x86_64
	CONTEXT context;
	EXCEPTION_POINTERS ex_ptrs;
	void* ebp;
	context.ContextFlags = CONTEXT_FULL;
	if (GetThreadContext(GetCurrentThread(), &context))
	{
		context.Eip = (DWORD)GetInstructionPtr();
		context.Ebp = (DWORD)&ebp;
		context.Esp = (DWORD)&context;
		ex_ptrs.ContextRecord = &context;
		ex_ptrs.ExceptionRecord = 0;
		return BuildStackTrace(&ex_ptrs, buffer, capacity, lineCapacity);
	}
	return 0;
}

StackTraceInfo StackTrace = {};