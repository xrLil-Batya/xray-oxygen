#include "stdafx.h"
#pragma hdrstop
#include "xr_input.h"
#include "IInputReceiver.h"
#include "../include/editor/ide.hpp"
#include "../FrayBuildConfig.hpp"
#include "OffSetOfWrapper.inl"
#include "IGame_AnselSDK.h"

CInput *	pInput = nullptr;

ENGINE_API float	psMouseSens = 1.f;
ENGINE_API float	psMouseSensScale = 1.f;
ENGINE_API Flags32	psMouseInvert = { FALSE };

float stop_vibration_time = flt_max;

CInput::CInput()
{
	Log("Starting INPUT device...");

	UINT deviceCountGet = 0;
	UINT rawInputDeviceCount = 0;
	deviceCountGet = GetRawInputDeviceList(NULL, &rawInputDeviceCount, sizeof(RAWINPUTDEVICELIST));
	R_ASSERT(rawInputDeviceCount > 0);

	xr_vector<RAWINPUTDEVICELIST> deviceLists;
	deviceLists.resize(rawInputDeviceCount);
	deviceCountGet = GetRawInputDeviceList(deviceLists.data(), &rawInputDeviceCount, sizeof(RAWINPUTDEVICELIST));

	R_ASSERT(deviceCountGet != -1);

	Msg("[CInput] Found input devices:");
	for (u32 i = 0; i < rawInputDeviceCount; i++)
	{
		string256 deviceName;
		UINT deviceNameSize = sizeof(deviceName);
		UINT charsCopied = GetRawInputDeviceInfoA(deviceLists[i].hDevice, RIDI_DEVICENAME, deviceName, &deviceNameSize);

		if (charsCopied > 0)
		{
			Msg("[CInput] Device%u \"%s\"", i, deviceName);
		}
		else
		{
			Msg("[CInput] Device%u \"ERROR\"", i);
		}
	}

	rawDevices[0].usUsagePage = 1;
	rawDevices[0].usUsage = 2; // mouse
	rawDevices[0].dwFlags = 0;
	rawDevices[0].hwndTarget = Device.m_hWnd;

	rawDevices[1].usUsagePage = 1;
	rawDevices[1].usUsage = 6; // keyboard
	rawDevices[1].dwFlags = 0;
	rawDevices[1].hwndTarget = Device.m_hWnd;
	BOOL bRegisteredInput = RegisterRawInputDevices(&rawDevices[0], 2, sizeof(RAWINPUTDEVICE));

	R_ASSERT(bRegisteredInput);

	Device.seqAppActivate.Add(this);
	Device.seqAppDeactivate.Add(this, REG_PRIORITY_HIGH);
	Device.seqFrame.Add(this, REG_PRIORITY_HIGH);
}

CInput::~CInput()
{
	rawDevices[0].usUsagePage = 1;
	rawDevices[0].usUsage = 2; // mouse
	rawDevices[0].dwFlags = RIDEV_REMOVE;
	rawDevices[0].hwndTarget = Device.m_hWnd;

	rawDevices[1].usUsagePage = 1;
	rawDevices[1].usUsage = 6; // keyboard
	rawDevices[1].dwFlags = RIDEV_REMOVE;
	rawDevices[1].hwndTarget = Device.m_hWnd;
	RegisterRawInputDevices(&rawDevices[0], 2, sizeof(rawDevices));

	Device.seqFrame.Remove(this);
	Device.seqAppDeactivate.Remove(this);
	Device.seqAppActivate.Remove(this);
}

void CInput::ProcessInput(LPARAM hRawInputParam)
{
	if (cbStack.empty()) return;
	HRAWINPUT hRawInput = (HRAWINPUT)hRawInputParam;

	RAWINPUT* pRawInput = (RAWINPUT*)&inputData[0];
	bool bShouldFreeMem = false;
	UINT rawInputSize = sizeof(inputData);

	GetRawInput:
	UINT RetBytes = GetRawInputData(hRawInput, RID_INPUT, pRawInput, &rawInputSize, sizeof(RAWINPUTHEADER));

	if (RetBytes != (UINT(-1)))
	{
		RAWINPUTHEADER& header = pRawInput->header;
		if (header.dwType == RIM_TYPEMOUSE)
		{
			RAWMOUSE& mouse = pRawInput->data.mouse;
			const bool IsAbsoluteInput = (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;

			if (!IsAbsoluteInput)
			{
				deltaMouse.set(mouse.lLastX, mouse.lLastY);
				cbStack.back()->IR_OnMouseMove(deltaMouse.x, deltaMouse.y);
			}

			USHORT mouseFlags = mouse.usButtonFlags;
			auto CheckMouseButtonState = [this, mouseFlags](u8 mouseKey, USHORT mouseDown, USHORT mouseUp)
			{
				if (mouseFlags & mouseDown)
				{
					pressedKeys[mouseKey] = true;
					cbStack.back()->IR_OnMousePress(mouseKey);
				}
				if (mouseFlags & mouseUp)
				{
					pressedKeys[mouseKey] = false;
					cbStack.back()->IR_OnMouseRelease(mouseKey);
				}
			};

			CheckMouseButtonState((u8)VK_LBUTTON, RI_MOUSE_LEFT_BUTTON_DOWN,	RI_MOUSE_LEFT_BUTTON_UP);
			CheckMouseButtonState((u8)VK_RBUTTON, RI_MOUSE_RIGHT_BUTTON_DOWN,	RI_MOUSE_RIGHT_BUTTON_UP);
			CheckMouseButtonState((u8)VK_MBUTTON, RI_MOUSE_MIDDLE_BUTTON_DOWN,	RI_MOUSE_MIDDLE_BUTTON_UP);
			CheckMouseButtonState((u8)VK_XBUTTON1, RI_MOUSE_BUTTON_4_DOWN,		RI_MOUSE_BUTTON_4_UP);
			CheckMouseButtonState((u8)VK_XBUTTON2, RI_MOUSE_BUTTON_5_DOWN,		RI_MOUSE_BUTTON_5_UP);
		}
		else if (header.dwType == RIM_TYPEKEYBOARD)
		{
			RAWKEYBOARD& keyboardKey = pRawInput->data.keyboard;

			USHORT& pressedKey = keyboardKey.VKey;
			R_ASSERT(pressedKey <= 0xFF);

			if (keyboardKey.Flags == RI_KEY_MAKE)
			{
				pressedKeys[pressedKey] = true;
				cbStack.back()->IR_OnKeyboardPress((u8)keyboardKey.VKey);
			}
			else if (keyboardKey.Flags & RI_KEY_BREAK)
			{
				pressedKeys[pressedKey] = false;
				cbStack.back()->IR_OnKeyboardRelease((u8)keyboardKey.VKey);
			}
		}
	}
	else
	{
		pRawInput = (RAWINPUT*)Memory.mem_alloc(rawInputSize);
		bShouldFreeMem = true;
		goto GetRawInput;
	}

	if (bShouldFreeMem)
	{
		Memory.mem_free(pRawInput);
	}
}

bool CInput::get_VK_name(u8 dik, LPSTR dest_str, int dest_sz)
{
	string128 keyName;
	int keyNameSize = GetKeyNameTextA(dik, keyName, sizeof(keyName));
	if (keyNameSize == 0)
	{
		return false;
	}
	memcpy(dest_str, keyName, keyNameSize);
	return true;
}

BOOL CInput::iGetAsyncKeyState(u8 dik)
{
	return pressedKeys[dik];
}

BOOL CInput::iGetAsyncBtnState(u8 btn)
{
	return pressedKeys[btn];
}

void CInput::iGetLastMouseDelta(Ivector2& p)
{
	p.set(deltaMouse.x, deltaMouse.y);
}

//-------------------------------------------------------
void CInput::iCapture(IInputReceiver *p)
{
	VERIFY(p);

	// change focus
	if (!cbStack.empty())
		cbStack.back()->IR_OnDeactivate();
	cbStack.push_back(p);
	cbStack.back()->IR_OnActivate();
}

void CInput::iRelease(IInputReceiver *p)
{
	if (p == cbStack.back())
	{
		cbStack.back()->IR_OnDeactivate();
		cbStack.pop_back();
		if (!cbStack.empty())
		{
			IInputReceiver * ir = cbStack.back();
			ir->IR_OnActivate();
		}
	}
	else
	{// we are not topmost receiver, so remove the nearest one
		size_t cnt = cbStack.size();
		for (; cnt > 0; --cnt)
			if (cbStack[cnt - 1] == p) 
			{
				xr_vector<IInputReceiver*>::iterator it = cbStack.begin();
				std::advance(it, (u32)cnt - 1);
				cbStack.erase(it);
				break;
			}
	}
}

void CInput::OnAppActivate(void)
{
	if (CurrentIR())
		CurrentIR()->IR_OnActivate();

	ZeroMemory(inputData, sizeof(inputData));
	ZeroMemory(pressedKeys, sizeof(pressedKeys));
}

void CInput::OnAppDeactivate(void)
{
	if (CurrentIR())
		CurrentIR()->IR_OnDeactivate();

	ZeroMemory(inputData, sizeof(inputData));
	ZeroMemory(pressedKeys, sizeof(pressedKeys));
}

void CInput::OnFrame(void)
{
	ScopeStatTimer frameTimer(Device.Statistic->Engine_InputFrame);
	if (cbStack.empty())
	{
		return;
	}
	dwCurTime = RDEVICE.TimerAsync_MMT();

	// check for holded keys

	for (u8 i = 0; i < 0xFF; i++)
	{
		if (pressedKeys[i])
		{
			cbStack.back()->IR_OnKeyboardHold(i);
		}
	}
}

IInputReceiver*	 CInput::CurrentIR()
{
	if (cbStack.size())
		return cbStack.back();
	else
		return NULL;
}

void  CInput::feedback(u16 s1, u16 s2, float time)
{
	stop_vibration_time = RDEVICE.fTimeGlobal + time;
}
