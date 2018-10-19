#include "stdafx.h"
#include "thread_utils.h"

xrCriticalSection::xrCriticalSection() : isLocked(false)
{
	InitializeCriticalSectionAndSpinCount(&Section, 250);
}

xrCriticalSection::~xrCriticalSection()
{
	DeleteCriticalSection(&Section);
}

void xrCriticalSection::Enter()
{
	isLocked = true;
	EnterCriticalSection(&Section);
}

void xrCriticalSection::Leave()
{
	isLocked = false;
	LeaveCriticalSection(&Section);
}

bool xrCriticalSection::TryLock()
{
	return isLocked;
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
