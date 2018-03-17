#pragma once

//TEMP

#define PIX_EVENT(Name)	dxPixEventWrapper	pixEvent##Name(L#Name)

class dxPixEventWrapper
{
public:
	dxPixEventWrapper(LPCWSTR wszName) { D3DPERF_BeginEvent( D3DCOLOR_RGBA(127,0,0,255), wszName );}
	~dxPixEventWrapper() {D3DPERF_EndEvent();}
};
