#pragma once
using std::swap;

  // string(char)
  //using xr_string = std::basic_string<char, std::char_traits<char>, xalloc<char>>;

  // vector
template <typename T, typename allocator = xalloc<T>>
using xr_vector = std::vector<T, allocator>;

template <typename T, typename Alloc>
void clearAndReserve(xr_vector<T, Alloc>& vec) 
{
    if (vec.capacity() <= (vec.size() / 2))
        vec.clear();
    else {
        vec.shrink_to_fit();
        vec.clear();
    }
}


// deque
template <typename T, typename allocator = xalloc<T>>
using xr_deque = std::deque<T, allocator>;

// queue
template <typename T, typename container = xr_deque<T>>
using xr_queue = std::queue<T, container>;

// stack
template <typename T, class C = xr_deque<T>>
using xr_stack = std::stack<T, C>;

template <typename T, typename allocator = xalloc<T>>
using xr_list = std::list<T, allocator>;

template <typename K, class P = std::less<K>, typename allocator = xalloc<K>>
using xr_set = std::set<K, P, allocator>;

template <typename K, class P = std::less<K>, typename allocator = xalloc<K>>
using xr_multiset = std::multiset<K, P, allocator>;

template <typename K, class V, class P = std::less<K>, typename allocator = xalloc<std::pair<const K, V>>>
using xr_map = std::map<K, V, P, allocator>;

template <typename K, class V, class P = std::less<K>, typename allocator = xalloc<std::pair<const K, V>>>
using xr_multimap = std::multimap<K, V, P, allocator>;

template <typename K, class V, class Traits = std::equal_to<K>,
    typename allocator = xalloc<std::pair<const K, V>>>
    using xr_hash_map = std::unordered_map<K, V, std::hash<K>, Traits, allocator>;

class XRCORE_API xr_string : public std::basic_string<char, std::char_traits<char>, xalloc<char>>
{
public:
    typedef std::basic_string<char, std::char_traits<char>, xalloc<char>> Super;

    xr_string(LPCSTR Str);
    xr_string(LPCSTR Str, u32 Size);
    xr_string(const xr_string& other);
    xr_string(const xr_string&& other);
    xr_string(const Super&& other);
    xr_string();

    xr_string& operator=(LPCSTR Str);
    xr_string& operator=(const xr_string& other);
    xr_string& operator=(const Super& other);

    template <size_t ArrayLenght>
    xr_string(char* (&InArray)[ArrayLenght])
    {
        assign(InArray, ArrayLenght);
    }

    xr_vector<xr_string> Split(char splitCh);
    xr_vector<xr_string> Split(u32 NumberOfSplits, ...);
	
    bool StartWith(const xr_string& Other) const;
    bool StartWith(LPCSTR Str) const;
    bool StartWith(LPCSTR Str, size_t Size) const;
    xr_string RemoveWhitespaces() const;

	static xr_string ToString(int Value);
};



namespace std {
    template<>
    class hash<xr_string> {
    public:
        size_t operator()(const xr_string &s) const
        {
            std::hash<xr_string::Super> hashFn;
            return hashFn(s);
        }
    };
}

struct pred_str {
    bool operator()(const char* x, const char* y) const { return std::strcmp(x, y) < 0; }
};
struct pred_stri {
    bool operator()(const char* x, const char* y) const { return _stricmp(x, y) < 0; }
};

#include "FixedVector.h"
#include "buffer_vector.h"

// auxilary definition
using LPSTRVec = xr_vector<LPSTR>;
using SStringVec = xr_vector<xr_string>;
using U8Vec = xr_vector<u8>;
using U16Vec = xr_vector<u16>;
using U32Vec = xr_vector<u32>;
using FloatVec = xr_vector<float>;
using IntVec = xr_vector<int>;
using boolVec = xr_vector<bool>;

template <typename K, class V, class Hasher = std::hash<K>, class Traits = std::equal_to<K>,
    typename allocator = xalloc<std::pair<const K, V>>>
    using xr_unordered_map = std::unordered_map<K, V, Hasher, Traits, allocator>;



