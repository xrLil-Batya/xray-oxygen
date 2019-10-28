//////////////////////////////////////////////
// Desc		: Strings utils
// Authors	: Giperion, ForserX
//////////////////////////////////////////////
// X-Ray Oxygen Engine 2016-2019
//////////////////////////////////////////////
#include <type_traits>

namespace StringUtils
{
	XRCORE_API const char* ConvertToUTF8(const shared_str& pStr, string64& OutUtf8String);
	XRCORE_API const char* ConvertToUTF8(const xr_string& pStr, string64& OutUtf8String);

template<typename StringType>
const wchar_t* ConvertToUnicode(const char* pStr, const u32 Lenght, StringType& OutString);

template<typename StringType>
const wchar_t* ConvertToUnicode(const shared_str& pStr, StringType& OutString);

template<typename StringType>
const wchar_t* ConvertToUnicode(const xr_string& pStr, StringType& OutString);

template<typename StringType, 
	typename InStringType,
	typename = std::enable_if_t<std::is_array<InStringType>::value>>
	const wchar_t* ConvertToUnicode(const InStringType& pStr, StringType& OutString)
{
	static_assert(std::is_same<std::remove_extent<StringType>::type, wchar_t>::value);
	VERIFY(sizeof(pStr) < (sizeof(OutString) / 2));
	MultiByteToWideChar(CP_ACP, 0, pStr, sizeof(pStr), OutString, sizeof(OutString) / 2);
	return &OutString[0];
}

template<typename StringType>
const wchar_t* ConvertToUnicode(const char* pStr, const u32 Lenght, StringType& OutString)
{
	static_assert(std::is_same<std::remove_extent<StringType>::type, wchar_t>::value);

	VERIFY(Lenght < (sizeof(OutString) / 2));
	MultiByteToWideChar(CP_ACP, 0, pStr, Lenght, OutString, sizeof(OutString) / 2);
	return &OutString[0];
}

template<typename StringType>
const wchar_t* ConvertToUnicode(const shared_str& pStr, StringType& OutString)
{
	return ConvertToUnicode<StringType>(pStr.c_str(), pStr.size(), OutString);
}

template<typename StringType>
const wchar_t* ConvertToUnicode(const xr_string& pStr, StringType& OutString)
{
	return ConvertToUnicode<StringType>(pStr.c_str(), pStr.size(), OutString);
}

}