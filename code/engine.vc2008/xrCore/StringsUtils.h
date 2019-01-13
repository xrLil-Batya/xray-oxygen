//////////////////////////////////////////////
// X-Ray Oxygen String Utils

const char* ConvertToUTF8(const shared_str& pStr, string64& OutUtf8String)
{
	wchar_t utfRusStr[64] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, pStr.c_str(), pStr.size(), utfRusStr, sizeof(utfRusStr));

	WideCharToMultiByte(CP_UTF8, 0, utfRusStr, sizeof(utfRusStr), OutUtf8String, sizeof(OutUtf8String), 0, 0);
	return &OutUtf8String[0];
};



