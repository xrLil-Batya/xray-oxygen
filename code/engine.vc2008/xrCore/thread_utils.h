#pragma once


class XRCORE_API xrCriticalSection
{
public:
	xrCriticalSection();
	~xrCriticalSection();

	void Enter();
	void Leave();

	inline void Lock()		{ this->Enter(); }
	inline void Unlock()	{ this->Leave(); }

	bool TryLock();

private:
	CRITICAL_SECTION Section;
	volatile bool isLocked;
};


class XRCORE_API xrCriticalSectionGuard
{
public:
	xrCriticalSectionGuard(xrCriticalSection& InSection);
	~xrCriticalSectionGuard();

private:
	xrCriticalSection& Section;
};
