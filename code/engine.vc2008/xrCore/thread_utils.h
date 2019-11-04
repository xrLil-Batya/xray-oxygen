#pragma once

class xrConditionalVariable;

class XRCORE_API xrCriticalSection
{
	friend class xrConditionalVariable;
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
	explicit xrCriticalSectionGuard(xrCriticalSection& InSection);
	~xrCriticalSectionGuard();

	// do not allow sharing
	xrCriticalSectionGuard(const xrCriticalSectionGuard& Other) = delete;
	xrCriticalSectionGuard& operator=(const xrCriticalSectionGuard& Other) = delete;

private:
	xrCriticalSection& Section;
};

// very slow due mem allocation
class XRCORE_API xrSharedCriticalSectionGuard
{
public:
	explicit xrSharedCriticalSectionGuard(xrCriticalSection& InSection);
	xrSharedCriticalSectionGuard(const xrSharedCriticalSectionGuard& InSharedSection);
	~xrSharedCriticalSectionGuard();

	xrSharedCriticalSectionGuard& operator=(const xrSharedCriticalSectionGuard& Other);

private:
	void ConditionalFreeLock();

	xr_atomic_u32* pReference;
	xrCriticalSection* pSection;
};

class XRCORE_API xrConditionalVariable
{
public:
	xrConditionalVariable();
	~xrConditionalVariable();

	void WakeOne();
	void WakeAll();

	void Sleep(xrCriticalSection& AcquiringCS);

private:
	CONDITION_VARIABLE Variable;
};