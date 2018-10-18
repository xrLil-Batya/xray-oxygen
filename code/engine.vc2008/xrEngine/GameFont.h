/******************************************************************************************************************
***** Authors: Lord
***** Modifed: 20.05.2018
***** Description: This file contain an implementation of Font Engine 
***** Copyright: GSC, OxyGen Team 2018 (C)
*******************************************************************************************************************/

#ifndef GameFontH
#define GameFontH
#pragma once
#include "MbHelpers.h"
#include "../Include/xrRender/FontRender.h"
 
class ENGINE_API CGameFont
{
	friend class dxFontRender;

public:
	enum EAligment
	{
		alLeft = 0,
		alRight,
		alCenter
	};

private:
	struct String
	{
		string1024 string;
		float x, y;
		float height;
		u32 c;
		EAligment align;
	};

	//

protected:

	float fCurrentHeight;
	float fCurrentX, fCurrentY;
	float fHeight;
	float fXStep;
	float fYStep;
	float fTCHeight;

	u32 nNumChars;
	u32 uFlags;
	u32 dwCurrentColor;

	Fvector2 vHalfPixel;
	Ivector2 vTS;
	EAligment eCurrentAlignment;
	Fvector2 vInterval;
	Fvector *TCMap;
	xr_vector<String> strings;
	IFontRender *pFontRender;

public:
	enum
	{
		fsGradient = (1 << 0),
		fsDeviceIndependent = (1 << 1),
		fsValid = (1 << 2),

		fsMultibyte = (1 << 3),

		fsForceDWORD = u32(-1)
	};

protected:
	inline const Fvector& GetCharTC(u16 c) { return TCMap[c]; }

public:
	CGameFont(LPCSTR section, u32 flags = 0);
	CGameFont(LPCSTR shader, LPCSTR texture, u32 flags = 0);
	~CGameFont();

	void Initialize(LPCSTR shader, LPCSTR texture);

	inline void SetColor(u32 C) { dwCurrentColor = C; };

	inline void SetHeightI(float S);
	inline void SetHeight(float S);

	inline float GetHeight() { return fCurrentHeight; };
	inline void SetInterval(float x, float y) { vInterval.set(x, y); };
	inline void SetInterval(const Fvector2& v) { vInterval.set(v); };
	inline void SetAligment(EAligment aligment) { eCurrentAlignment = aligment; }

	float SizeOf_(LPCSTR s);
	float SizeOf_(const wide_char *wsStr);

	float SizeOf_(const char cChar);  // only ANSII

	float CurrentHeight_();

	void OutSetI(float x, float y);
	void OutSet(float x, float y);

	void MasterOut(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, float _x, float _y, float _skip, LPCSTR fmt, va_list p);

	u32 smart_strlen(const char* S);
	BOOL IsMultibyte() { return (uFlags & fsMultibyte); };
	u16 SplitByWidth(u16 * puBuffer, u16 uBufferSize, float fTargetWidth, const char * pszText);
	u16 GetCutLengthPos(float fTargetWidth, const char * pszText);

	void OutI(float _x, float _y, LPCSTR fmt, ...);
	void Out(float _x, float _y, LPCSTR fmt, ...);
	void OutNext(LPCSTR fmt, ...);

	void OutSkip(float val = 1.f);

	void OnRender();

	inline void Clear() { strings.clear(); };

	shared_str m_font_name;
};

#endif // _XR_GAMEFONT_H_
