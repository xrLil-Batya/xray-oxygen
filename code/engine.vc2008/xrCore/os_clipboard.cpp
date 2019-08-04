////////////////////////////////////////////////////////////////////////////
//	Module 		: os_clipboard.cpp
//	Created 	: 21.02.2008
//	Author		: Evgeniy Sokolov
//	Description : os clipboard class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "os_clipboard.h"

void os_clipboard::copy_to_clipboard(const char* buf)
{
#if PLATFORM == _WINDOWS
	if (OpenClipboard(nullptr))
	{
		const size_t handle_size = (xr_strlen(buf) + 1) * sizeof(char);
		const HGLOBAL handle = GlobalAlloc(GHND, handle_size);

		if (handle)
		{
			char* memory = (char*)GlobalLock(handle);
			xr_strcpy(memory, handle_size, buf);
			GlobalUnlock(handle);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, handle);
		}
		CloseClipboard();
	}
#endif
}

void os_clipboard::paste_from_clipboard(char* buffer, u32 const& buffer_size)
{
#if PLATFORM == _WINDOWS
	VERIFY(buffer);
	VERIFY(buffer_size > 0);

	if (OpenClipboard(nullptr))
	{
		const HGLOBAL hmem = GetClipboardData(CF_TEXT);
		if (hmem)
		{
			const char* clipdata = (const char*)GlobalLock(hmem);
			strncpy_s(buffer, buffer_size, clipdata, buffer_size - 1);
			buffer[buffer_size - 1] = 0;

			for (size_t i = 0; i < strlen(buffer); ++i)
			{
				char c = buffer[i];
				if (((isprint(c) == 0) && (c != char(-1))) || c == '\t' || c == '\n')// "я" = -1
				{
					buffer[i] = ' ';
				}
			}
			GlobalUnlock(hmem);
		}
		CloseClipboard();
	}
#endif
}

void os_clipboard::update_clipboard(const char* string)
{
#if PLATFORM == _WINDOWS
	if (OpenClipboard(nullptr))
	{
		const HGLOBAL handle = GetClipboardData(CF_TEXT);
		if (handle)
		{
			char* memory = (char*)GlobalLock(handle);
			size_t memory_length = strlen(memory);
			size_t string_length = strlen(string);
			size_t buffer_size = (memory_length + string_length + 1) * sizeof(char);
			char* buffer = (char*)_alloca(buffer_size);
			xr_strcpy(buffer, buffer_size, memory);
			GlobalUnlock(handle);

			xr_strcat(buffer, buffer_size, string);
			CloseClipboard();
			copy_to_clipboard(buffer);
		}
		else
		{
			CloseClipboard();
			copy_to_clipboard(string);
		}
	}
#endif
}