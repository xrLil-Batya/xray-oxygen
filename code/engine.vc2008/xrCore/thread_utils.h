#pragma once


class XRCORE_API xrCriticalSection
{
public:
	xrCriticalSection();
	~xrCriticalSection();

	void Enter();
	void Leave();

private:

	CRITICAL_SECTION Section;
};


class XRCORE_API xrCriticalSectionGuard
{
public:
	xrCriticalSectionGuard(xrCriticalSection& InSection);
	~xrCriticalSectionGuard();

private:
	xrCriticalSection& Section;
};
