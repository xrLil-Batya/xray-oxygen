// Giperion October 2019
// [EUREKA] 3.11.1

//////////////////////////////////////////////////////////////
// Desc		: Helper for automatic control of handle lifetime
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#pragma once

template<typename HandleType, class HandleOperators>
struct ScopeHandle
{
	ScopeHandle(const HandleType& InHandle)
		: StoredHandle(InHandle)
	{}

	operator HandleType() const
	{
		return StoredHandle;
	}

	ScopeHandle& operator=(const HandleType& InHandle)
	{
		CloseHandle();
		StoredHandle = InHandle;
	}

	HandleType Get() const
	{
		return StoredHandle;
	}

	bool IsValid() const
	{
		return HandleOperators::IsValidHandle(StoredHandle);
	}

	~ScopeHandle()
	{
		CloseHandle();
	}

private:

	void CloseHandle()
	{
		if (HandleOperators::IsValidHandle(StoredHandle))
		{
			HandleOperators::CloseHandle(StoredHandle);
			StoredHandle = HandleOperators::GetInvalidHandle();
		}
	}

	HandleType StoredHandle;
};

#if PLATFORM == _WINDOWS || PLATFORM == _XBOX_ONE
// both platforms use same windows shit

class WindowsHandleOperators
{
public:
	static bool IsValidHandle(HANDLE handle);
	static void CloseHandle(HANDLE handle);
	static HANDLE GetInvalidHandle();
};

using WinScopeHandle = ScopeHandle<HANDLE, WindowsHandleOperators>;

#endif