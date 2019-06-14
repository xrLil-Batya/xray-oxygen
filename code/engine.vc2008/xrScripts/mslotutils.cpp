#include "stdafx.h"
#include "mslotutils.hpp"

inline void CMailSlotMsg::Read(void* dst, int sz) {
	std::memcpy(dst, (void*)(&m_buff[0] + m_pos), sz);
	m_pos += sz;
};
inline void CMailSlotMsg::Write(const void* src, int sz) {
	std::memcpy((void*)(&m_buff[0] + m_pos), src, sz);
	m_pos += sz; m_len = m_pos;
};

CMailSlotMsg::CMailSlotMsg()
{
	Reset();
};
inline void  CMailSlotMsg::Reset()
{
	m_len = 0;
	m_pos = 0;
	std::memset(m_buff, 0, 2048);
};
inline void  CMailSlotMsg::SetBuffer(const char* b, int sz)
{
	Reset();
	std::memcpy(m_buff, b, sz);
	m_len = sz;
	m_pos = 0;
};
inline void* CMailSlotMsg::GetBuffer()
{
	return m_buff;
};
inline void	 CMailSlotMsg::SetLen(DWORD l)
{
	m_len = l;
};
inline DWORD CMailSlotMsg::GetLen() const
{
	return m_len;
};
inline bool	CMailSlotMsg::r_string(char* dst) {
	int sz;
	r_int(sz);
	Read(dst, sz + 1);
	return true;
};

inline bool	CMailSlotMsg::w_string(const char* dst) {
	size_t sz = xr_strlen(dst);
	w_int((int)sz);
	Write(dst, (int)(sz + 1)); 
	return true;
};

inline bool	CMailSlotMsg::r_float(float& dst) {
	Read(&dst, sizeof(float));
	return true;
};

inline bool	CMailSlotMsg::w_float(const float src) {
	Write(&src, sizeof(float));
	return true;
};

inline bool	CMailSlotMsg::r_int(int& dst) {
	Read(&dst, sizeof(int));
	return true;
};

inline bool	CMailSlotMsg::w_int(const int src) {
	Write(&src, sizeof(int));
	return true;
};

inline bool	CMailSlotMsg::r_buff(void* dst, int sz) {
	Read(dst, sz);
	return true;
};

inline bool	CMailSlotMsg::w_buff(void* src, int sz) {
	Write(src, sz);
	return true;
};

inline void* CreateMailSlotByName(char* slotName)
{
	return CreateMailslotA(slotName, 0 /* no maximum message size */, 
	MAILSLOT_WAIT_FOREVER/* no time-out for operations */, 
		(LPSECURITY_ATTRIBUTES)nullptr); /* no security attributes */
}
inline bool CheckExisting(char* slotName)
{
	void* hFile;
	bool res;
	hFile = CreateFileA(slotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,  // required to write to a mailslot 
		(LPSECURITY_ATTRIBUTES)nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(void*)nullptr);

	res = (hFile != INVALID_HANDLE_VALUE);

	if (res)
		CloseHandle(hFile);

	return res;
}
inline bool SendMailslotMessage(char* slotName, CMailSlotMsg& msg) {
	bool fResult;
	void* hFile;
	DWORD cbWritten;

	hFile = CreateFile(slotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,  // required to write to a mailslot 
		(LPSECURITY_ATTRIBUTES)nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(void*)nullptr);

	R_ASSERT(hFile != INVALID_HANDLE_VALUE);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;


	fResult = !!WriteFile(hFile,
		msg.GetBuffer(),
		msg.GetLen(),
		&cbWritten,
		(LPOVERLAPPED)nullptr);

	R_ASSERT(fResult);
	fResult = !!CloseHandle(hFile);
	R_ASSERT(fResult);
	return fResult;
}

inline bool CheckMailslotMessage(void* hSlot, CMailSlotMsg& msg) {
	DWORD cbMessage, cMessage, cbRead;
	bool fResult;
	void* hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	hEvent = CreateEventA(nullptr, false, false, "__Slot");
	if (!hEvent)
		return false;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;


	fResult = GetMailslotInfo(hSlot, // mailslot void* 
		(LPDWORD)nullptr,               // no maximum message size 
		&cbMessage,                   // size of next message 
		&cMessage,                    // number of messages 
		(LPDWORD)nullptr);              // no read time-out 

	R_ASSERT(fResult);

	if (!fResult || cbMessage == MAILSLOT_NO_MESSAGE) {
		CloseHandle(hEvent);
		return false;
	}

	msg.Reset();
	fResult = ReadFile(hSlot,
		msg.GetBuffer(),
		cbMessage,
		&cbRead,
		&ov);
	msg.SetLen(cbRead);
	R_ASSERT(fResult);

	CloseHandle(hEvent);
	return fResult;
}
