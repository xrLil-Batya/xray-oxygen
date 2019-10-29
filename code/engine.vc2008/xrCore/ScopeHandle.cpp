#include "stdafx.h"

bool WindowsHandleOperators::IsValidHandle(HANDLE handle)
{
	return handle != INVALID_HANDLE_VALUE;
}

void WindowsHandleOperators::CloseHandle(HANDLE handle)
{
	CloseHandle(handle);
}

HANDLE WindowsHandleOperators::GetInvalidHandle()
{
	return INVALID_HANDLE_VALUE;
}
