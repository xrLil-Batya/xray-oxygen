//////////////////////////////////////////////
// X-Ray Oxygen String Utils

const char* ConvertToUTF8(const char* pStr)
{
	const int pStrLen = (int)strlen(pStr);
	wchar_t utfRusStr[64] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, pStr, pStrLen, utfRusStr, sizeof(utfRusStr));

	char OutStr[64] = { 0 };
	WideCharToMultiByte(CP_UTF8, 0, utfRusStr, sizeof(utfRusStr), OutStr, sizeof(OutStr), 0, 0);
	return OutStr;
};