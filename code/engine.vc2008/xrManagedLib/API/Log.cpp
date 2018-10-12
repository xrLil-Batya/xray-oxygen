#include "stdafx.h"
#include "Log.h"

namespace XRay
{
	void Log::Info(String^ message)
	{
		if (message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(message);
		Msg("%S", pinMessage);

#ifdef DEBUG
		Console::WriteLine("{0}", message);
#endif // DEBUG
	}

	void Log::Warning(String^ message)
	{
		if (message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(message);
		Msg("* %S", pinMessage);

#ifdef DEBUG
		Console::WriteLine("* {0}", message);
#endif // DEBUG
	}

	void Log::Error(String^ message)
	{
		if (message == nullptr) return;
		pin_ptr<const wchar_t> pinMessage = PtrToStringChars(message);
		Msg("! %S", pinMessage);

#ifdef DEBUG
		Console::WriteLine("! {0}", message);
#endif // DEBUG
	}
}