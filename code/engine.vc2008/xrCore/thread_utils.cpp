#include "stdafx.h"
#include "thread_utils.h"

xrCriticalSection::xrCriticalSection()
{
	InitializeCriticalSectionAndSpinCount(&Section, 250);
}

xrCriticalSection::~xrCriticalSection()
{
	DeleteCriticalSection(&Section);
}

void xrCriticalSection::Enter()
{
	EnterCriticalSection(&Section);
}

void xrCriticalSection::Leave()
{
	LeaveCriticalSection(&Section);
}

xrCriticalSectionGuard::xrCriticalSectionGuard(xrCriticalSection& InSection)
	: Section(InSection)
{
	Section.Enter();
}

xrCriticalSectionGuard::~xrCriticalSectionGuard()
{
	Section.Leave();
}
