#pragma once
#include "xrScripts.h"

#define	_memcpy std::memcpy
#define	_memset std::memset
#define	_strlen xr_strlen

class SCRIPT_API CMailSlotMsg 
{
	char	m_buff [2048];
	DWORD	m_len;
	int     m_pos;
	inline void Read(void* dst, int sz);
	inline void Write(const void* src, int sz);

public:
	CMailSlotMsg();
	inline void  Reset();
	inline void  SetBuffer(const char* b, int sz);
	inline void* GetBuffer();
	inline void	 SetLen(DWORD l);
	inline DWORD GetLen() const;
	
	inline bool	r_string(char* dst);
	inline bool	w_string(const char* dst);
	inline bool	r_float(float& dst);
	inline bool	w_float(const float src);
	inline bool	r_int(int& dst);
	inline bool	w_int(const int src);
	inline bool	r_buff(void* dst, int sz);
	inline bool	w_buff(void* src, int sz);
};

SCRIPT_API inline HANDLE CreateMailSlotByName(LPSTR slotName);
SCRIPT_API inline bool CheckExisting(LPSTR slotName);
SCRIPT_API inline bool SendMailslotMessage(LPSTR slotName, CMailSlotMsg& msg);
SCRIPT_API inline bool CheckMailslotMessage(HANDLE hSlot, CMailSlotMsg& msg);