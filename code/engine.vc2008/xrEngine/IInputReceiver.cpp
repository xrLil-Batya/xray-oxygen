#include "stdafx.h"
#include "xr_input.h"
#include "iinputreceiver.h"

void	IInputReceiver::IR_Capture						()
{
	VERIFY(pInput);
	pInput->iCapture(this);
}

void	IInputReceiver::IR_Release						()
{
	VERIFY(pInput);
	pInput->iRelease(this);
}

void	IInputReceiver::IR_GetLastMouseDelta			(Ivector2& p)
{
	VERIFY(pInput);
	pInput->iGetLastMouseDelta( p );
}

void IInputReceiver::IR_OnDeactivate					()
{}

void IInputReceiver::IR_OnActivate()
{}

BOOL IInputReceiver::IR_GetKeyState(u8 dik)
{
	VERIFY(pInput);
	return pInput->iGetAsyncKeyState(dik);
}

void	IInputReceiver::IR_GetMousePosScreen			(Ivector2& p)
{
	GetCursorPos((LPPOINT)&p);
}
void	IInputReceiver::IR_GetMousePosReal				(HWND hwnd, Ivector2 &p)
{
	IR_GetMousePosScreen(p);
	if (hwnd) ScreenToClient(hwnd,(LPPOINT)&p);
}
void	IInputReceiver::IR_GetMousePosReal				(Ivector2 &p)
{
	IR_GetMousePosReal(RDEVICE.m_hWnd,p);
}
void	IInputReceiver::IR_GetMousePosIndependent		(Fvector2 &f)
{
	Ivector2 p;
	IR_GetMousePosReal(p);
	f.set(
		2.f*float(p.x)/float(RDEVICE.dwWidth)-1.f,
		2.f*float(p.y)/float(RDEVICE.dwHeight)-1.f
		);
}
void	IInputReceiver::IR_GetMousePosIndependentCrop	(Fvector2 &f)
{
	IR_GetMousePosIndependent(f);
	if (f.x<-1.f) f.x=-1.f;
	if (f.x> 1.f) f.x= 1.f;
	if (f.y<-1.f) f.y=-1.f;
	if (f.y> 1.f) f.y= 1.f;
}
