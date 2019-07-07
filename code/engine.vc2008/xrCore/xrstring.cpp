#include "stdafx.h"
#pragma hdrstop
#pragma warning(disable: 4366)
#include "xrstring.h"

XRCORE_API str_container	g_pStringContainer;

struct str_container_impl
{
	xr_map<u32, str_value*> buffer;

    str_container_impl()
    {}

    str_value*       find(str_value* value, const char* str)
    {
		auto strNode = buffer.find(value->dwCRC);
		if (strNode != buffer.end())
		{
			str_value* strNodeValue = strNode->second;
			while (strNodeValue != nullptr)
			{
				if (value->dwLength == strNodeValue->dwLength &&
					memcmp(strNodeValue->value, str, strNodeValue->dwLength) == 0)
				{
					return strNodeValue;
				}
				strNodeValue = strNodeValue->nextNode;
			}
		}

		return nullptr;
    }

    void			 insert(str_value* value)
    {
		auto bufIter = buffer.find(value->dwCRC);
		if (bufIter != buffer.end())
		{
			// node with same crc32 founded
			// let's make sanity check first
			str_value* node = bufIter->second;
			while (node->nextNode != nullptr)
			{
				VERIFY(xr_strcmp(bufIter->second->value, value->value) != 0);
				node = node->nextNode;
			}

			node->nextNode = value;
		}
		else
		{
			// no strings with this crc32 - add this one
			buffer[value->dwCRC] = value;
		}
    }

    void			 clean()
    {
		for (auto iter = buffer.begin(); iter != buffer.end();)
		{
			str_value* prevNode = nullptr;
			str_value* strValueNode = iter->second;
			while (strValueNode != nullptr)
			{
				if (strValueNode->dwReference == 0)
				{
					str_value* nodeToDelete = strValueNode;
					strValueNode = strValueNode->nextNode;
					xr_free(nodeToDelete);
					if (prevNode != nullptr)
					{
						prevNode->nextNode = strValueNode;
					}

					if (strValueNode == nullptr)
					{
						if (prevNode == nullptr)
						{
							iter = buffer.erase(iter);
							break;
						}
					}
					else
					{
						iter->second = strValueNode;
					}
				}
				else
				{
					prevNode = strValueNode;
					strValueNode = strValueNode->nextNode;
				}
			}

			// additional check, because "erase" can return end
			if (iter == buffer.end())
				break;

			iter++;
		}
    }

    void			 verify()
    {
//         Msg("strings verify started");
//         for (str_value* value : buffer)
//         {
//             while (value)
//             {
//                 u32			crc = crc32(value->value, value->dwLength);
//                 string32	crc_str;
//                 R_ASSERT3(crc == value->dwCRC, "CorePanic: read-only memory corruption (shared_strings)", itoa(value->dwCRC, crc_str, 16));
//                 R_ASSERT3(value->dwLength == xr_strlen(value->value), "CorePanic: read-only memory corruption (shared_strings, internal structures)", value->value);
//                 value = value->next;
//             }
//         }
//         Msg("strings verify completed");
    }

    void			dump(FILE* f) const
    {
//         for (str_value* value : buffer)
//         {
//             while (value)
//             {
//                 fprintf(f, "ref[%4u]-len[%3u]-crc[%8X] : %s\n", value->dwReference, value->dwLength, value->dwCRC, value->value);
//                 value = value->next;
//             }
//         }
    }

    void			dump(IWriter* f) const
    {
//         for (str_value* value : buffer)
//         {
//             string4096		temp;
//             while (value)
//             {
//                 xr_sprintf(temp, sizeof(temp), "ref[%4d]-len[%3d]-crc[%8X] : %s\n", value->dwReference, value->dwLength, value->dwCRC, value->value);
//                 f->w_string(temp);
//                 value = value->next;
//             }
//         }
    }

    int				stat_economy()
    {
        int counter = 0;
		for (auto pair : buffer)
		{
			counter += pair.second->dwLength + 1;
		}

        return counter;
    }

	bool isNodePresented(shared_str const* pNode)
	{
		if (pNode == nullptr) return false;
		if (const str_value* strNode = pNode->_get())
		{
			auto bufferNodeIter = buffer.find(strNode->dwCRC);
			if (bufferNodeIter != buffer.end())
			{
				str_value* bufferStrNode = bufferNodeIter->second;

				while (bufferStrNode != nullptr)
				{
					if (bufferStrNode->dwLength == strNode->dwLength &&
						memcmp(bufferStrNode->value, strNode->value, bufferStrNode->dwLength) == 0)
					{
						return true;
					}
					bufferStrNode = bufferStrNode->nextNode;
				}
			}
		}

		return false;
	}

};

str_container::str_container()
{
    impl = new str_container_impl();
	constexpr size_t emptySize = sizeof(str_value) + 1;
	pEmpty = (str_value*)Memory.mem_alloc(emptySize);
	ZeroMemory(pEmpty, emptySize);
	pEmpty->dwCRC = crc32("", 1);
	pEmpty->dwReference = 9999;
}

str_value*	str_container::dock(str_c value)
{
    if (!value) return nullptr;

	xrCriticalSectionGuard guard(cs);

    str_value*	result = nullptr;

    // calc len
    u32		s_len = xr_strlen(value);
    u32		s_len_with_zero = (u32)s_len + 1;
    VERIFY(sizeof(str_value) + s_len_with_zero < 4096);

    // setup find structure
    char	header[sizeof(str_value)];
    str_value*	sv = (str_value*)header;
    sv->dwReference = 0;
    sv->dwLength = s_len;
    sv->dwCRC = crc32(value, s_len);
	sv->nextNode = nullptr;

    // search
    result = impl->find(sv, value);

    // it may be the case, string is not found or has "non-exact" match
    if (result == nullptr) 
	{
        result = (str_value*)Memory.mem_alloc(sizeof(str_value) + s_len_with_zero);

        result->dwReference = 0;
        result->dwLength = sv->dwLength;
        result->dwCRC = sv->dwCRC;
		result->nextNode = nullptr;
        std::memcpy(result->value, value, s_len_with_zero);

        impl->insert(result);
    }

    return	result;
}

void		str_container::clean()
{
	xrCriticalSectionGuard guard(cs);
    impl->clean();
}

void		str_container::verify()
{
	xrCriticalSectionGuard guard(cs);
    impl->verify();
}

void		str_container::dump()
{
	xrCriticalSectionGuard guard(cs);
    FILE* F = fopen("d:\\$str_dump$.txt", "w");
    impl->dump(F);
    fclose(F);
}

void		str_container::dump(IWriter* W)
{
	xrCriticalSectionGuard guard(cs);
    impl->dump(W);
}

u32			str_container::stat_economy()
{
	xrCriticalSectionGuard guard(cs);
    int				counter = 0;
    counter -= sizeof(*this);
    counter += impl->stat_economy();
    return			u32(counter);
}

bool str_container::isNodePresented(shared_str const* pNode)
{
	xrCriticalSectionGuard guard(cs);
	return impl->isNodePresented(pNode);
}

str_value* str_container::getEmpty() const
{
	return pEmpty;
}

str_container::~str_container()
{
	xrCriticalSectionGuard guard(cs);
    clean();
    xr_delete(impl);
}

xrSharedCriticalSectionGuard str_container::acquireLock()
{
	return xrSharedCriticalSectionGuard(cs);
}

void str_container::enterLock()
{
	cs.Enter();
}

void str_container::leaveLock()
{
	cs.Leave();
}

//xr_string class
xr_vector<xr_string> xr_string::Split(char splitCh)
{
	xr_vector<xr_string> Result;

	u32 SubStrBeginCursor = 0;
	u32 Len = 0;

	u32 StrCursor = 0;
	for (; StrCursor < size(); ++StrCursor)
	{
		if (at(StrCursor) == splitCh)
		{
			if ((StrCursor - SubStrBeginCursor) > 0)
			{
				Len = StrCursor - SubStrBeginCursor;
				Result.emplace_back(xr_string(&at(SubStrBeginCursor), Len));
				SubStrBeginCursor = StrCursor + 1;
			}
			else
			{
				Result.emplace_back("");
				SubStrBeginCursor = StrCursor + 1;
			}
		}
	}

	if (StrCursor > SubStrBeginCursor)
	{
		Len = StrCursor - SubStrBeginCursor;
		Result.emplace_back(xr_string(&at(SubStrBeginCursor), Len));
	}
    return Result;
}

xr_string::xr_string()
    : Super()
{
}

xr_string::xr_string(LPCSTR Str, u32 Size)
    : Super(Str, Size)
{
}

xr_string::xr_string(const xr_string& other)
    : Super(other)
{
}

xr_string::xr_string(const xr_string&& other)
    : Super(other)
{
}

xr_string::xr_string(const Super&& other)
    : Super(other)
{
}

xr_string::xr_string(LPCSTR Str)
    : Super(Str)
{
}

xr_string& xr_string::operator=(LPCSTR Str)
{
    Super::operator=(Str);
     return *this;
}

xr_string& xr_string::operator=(const xr_string& other)
{
    Super::operator=(other);
    return *this;
}

xr_string& xr_string::operator=(const Super& other)
{
    Super::operator=(other);
    return *this;
}

xr_vector<xr_string> xr_string::Split(u32 NumberOfSplits, ...)
{
    xr_vector<xr_string> intermediateTokens;
    xr_vector<xr_string> Result;

    va_list args;
    va_start(args, NumberOfSplits);

    for (u32 i = 0; i < NumberOfSplits; ++i)
    {
        char splitCh = va_arg(args, char);
        
        //special case for first try
        if (i == 0)
        {
            Result = Split(splitCh);
        }

        for (xr_string& str : Result)
        {
            xr_vector<xr_string> TokenStrResult = str.Split(splitCh);
            intermediateTokens.insert(intermediateTokens.end(), TokenStrResult.begin(), TokenStrResult.end());
        }

        if (!intermediateTokens.empty())
        {
            Result.clear();
            Result.insert(Result.begin(), intermediateTokens.begin(), intermediateTokens.end());
            intermediateTokens.clear();
        }
    }

    va_end(args);

    return Result;
}


xr_string xr_string::RemoveWhitespaces() const
{
    size_t Size = size();
    if (Size == 0) return xr_string();

    xr_string Result;
    Result.reserve(Size);

    const char* OrigStr = data();

    for (size_t i = 0; i < Size; ++i)
    {
        if (*OrigStr != ' ')
        {
            Result.push_back(OrigStr[i]);
        }
    }

    return Result;
}

bool xr_string::StartWith(const xr_string& Other) const
{
    return StartWith(Other.data(), Other.size());
}


bool xr_string::StartWith(LPCSTR Str) const
{
    u32 StrLen = xr_strlen(Str);
    return StartWith(Str, (int)StrLen);
}

bool xr_string::StartWith(LPCSTR Str, size_t Size) const
{
    size_t OurSize = size();

    //String is greater then our, we can't success
    if (OurSize < Size) return false;

    const char* OurStr = data();

    for (int i = 0; i < Size; ++i)
    {
        if (OurStr[i] != Str[i])
        {
            return false;
        }
    }

    return true;
}

xr_string xr_string::ToString(int Value)
{
	string64 buf = {0};
	itoa(Value, &buf[0], 10);

	return xr_string(buf);
}

xr_string xr_string::ToString(unsigned int Value)
{
	string64 buf = { 0 };
	sprintf(buf, "%u", Value);

	return xr_string(buf);
}

xr_string xr_string::ToString(float Value)
{
	string64 buf = { 0 };
	sprintf(buf, "%f", Value);

	return xr_string(buf);
}

xr_string xr_string::ToString(double Value)
{
	string64 buf = { 0 };
	sprintf(buf, "%f", Value);

	return xr_string(buf);
}

xr_string xr_string::Join(xrStringVector::iterator beginIter, xrStringVector::iterator endIter, const char delimeter /*= '\0'*/)
{
	xr_string Result;
	xrStringVector::iterator cursorIter = beginIter;

	while (cursorIter != endIter)
	{
		Result.append(*cursorIter);
		if (delimeter != '\0')
		{
			Result.push_back(delimeter);
		}
		cursorIter++;
	}

	if (delimeter != '\0')
	{
		Result.erase(Result.end() - 1);
	}

	return Result;
}