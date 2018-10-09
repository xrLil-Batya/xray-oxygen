#include "stdafx.h"
#include "Log.h"

namespace XRay
{
	void Log::Info(String^ Message)
	{
		if (Message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(Message);
		Msg("%S", pinMessage);
	}

	void Log::Warning(String^ Message)
	{
		if (Message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(Message);
		Msg("* %S", pinMessage);
	}

	void Log::Error(String^ Message)
	{
		if (Message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(Message);
		Msg("! %S", pinMessage);
	}
}