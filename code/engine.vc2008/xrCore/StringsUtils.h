//////////////////////////////////////////////
// Desc		: Strings utils
// Authors	: Giperion, ForserX
//////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////

const char* ConvertToUTF8(const shared_str& pStr, string64& OutUtf8String)
{
	R_ASSERT(pStr.size() < 64);
	wchar_t utfRusStr[64] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, pStr.c_str(), pStr.size(), utfRusStr, sizeof(utfRusStr));

	WideCharToMultiByte(CP_UTF8, 0, utfRusStr, sizeof(utfRusStr), OutUtf8String, sizeof(OutUtf8String), 0, 0);
	return &OutUtf8String[0];
};

const char* ConvertToUTF8(const xr_string& pStr, string64& OutUtf8String)
{
	R_ASSERT(pStr.size() < 64);
	wchar_t utfRusStr[64] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, pStr.c_str(), pStr.size(), utfRusStr, sizeof(utfRusStr));

	WideCharToMultiByte(CP_UTF8, 0, utfRusStr, sizeof(utfRusStr), OutUtf8String, sizeof(OutUtf8String), 0, 0);
	return &OutUtf8String[0];
};