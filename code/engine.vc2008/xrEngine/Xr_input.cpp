#include "stdafx.h"
#include "xr_input.h"
#include "IInputReceiver.h"
#include "../include/editor/ide.hpp"
#include "../FrayBuildConfig.hpp"
#include "IGame_AnselSDK.h"
#include "xr_level_controller.h"
#include <Xinput.h>

CInput *	pInput = nullptr;

ENGINE_API float	psMouseSens = 1.f;
ENGINE_API float	psMouseSensScale = 1.f;
ENGINE_API Flags32	psMouseInvert = { FALSE };

float stop_vibration_time = flt_max;

CInput::CInput()
{
	Log("Starting INPUT device...");

	bGamepadConnected = false;
	bVibrationStarted = false;
	bAllowBorderAccess = true;
	gamepadUserIndex = 0;
	gamepadLastPacketId = 0;
	leftTrigger = 0.0f;
	rightTrigger = 0.0f;
	leftThumbstick.setZero();
	rightThumbstick.setZero();
	UINT deviceCountGet = 0;
	UINT rawInputDeviceCount = 0;
	ResetPressedState();
	deviceCountGet = GetRawInputDeviceList(NULL, &rawInputDeviceCount, sizeof(RAWINPUTDEVICELIST));
	R_ASSERT(rawInputDeviceCount > 0);

	xr_vector<RAWINPUTDEVICELIST> deviceLists;
	deviceLists.resize(rawInputDeviceCount);
	deviceCountGet = GetRawInputDeviceList(deviceLists.data(), &rawInputDeviceCount, sizeof(RAWINPUTDEVICELIST));

	R_ASSERT(deviceCountGet != -1);

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
	Device.seqResolutionChanged.Add(this, REG_PRIORITY_HIGH);
}

CInput::~CInput()
{
	ClipCursor(NULL);
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
			RAWMOUSE& refMouseData = pRawInput->data.mouse;
			const bool IsAbsoluteInput = (refMouseData.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;

			if (refMouseData.usButtonFlags & RI_MOUSE_WHEEL)
			{
				cbStack.back()->IR_OnMouseWheel(int(refMouseData.usButtonData << 16));
			}
			else if (!IsAbsoluteInput)
			{
				deltaMouse.set(refMouseData.lLastX, refMouseData.lLastY);
				cbStack.back()->IR_OnMouseMove(deltaMouse.x, deltaMouse.y);
			}

			USHORT mouseFlags = refMouseData.usButtonFlags;
			auto CheckMouseButtonState = [this, mouseFlags](u8 mouseKey, USHORT mouseDown, USHORT mouseUp)
			{
				if (mouseFlags & mouseDown)
				{
					pressedKeys[mouseKey] = true;
					cbStack.back()->IR_OnKeyboardPress(mouseKey);
				}
				if (mouseFlags & mouseUp)
				{
					pressedKeys[mouseKey] = false;
					cbStack.back()->IR_OnKeyboardRelease(mouseKey);
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

			if (keyboardKey.Flags == RI_KEY_MAKE || keyboardKey.Flags == RI_KEY_E0)
			{
				if (!pressedKeys[pressedKey])
				{
					pressedKeys[pressedKey] = true;
					cbStack.back()->IR_OnKeyboardPress((u8)pressedKey);
				}
			}
			else if (keyboardKey.Flags & RI_KEY_BREAK)
			{
				if (pressedKeys[pressedKey])
				{
					pressedKeys[pressedKey] = false;
					cbStack.back()->IR_OnKeyboardRelease((u8)pressedKey);
				}
			}

			if (pressedKeys[VK_F4] && pressedKeys[VK_MENU])
			{
				Engine.Event.Defer("KERNEL:disconnect");
				Engine.Event.Defer("KERNEL:quit");

				return;
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
	const xr_string& keyName = KeyNamesTable.at(dik);

	memcpy(dest_str, keyName.c_str(), keyName.size());
	dest_str[keyName.size()] = '\0';
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
	ResetPressedState();

	// change focus
	if (!cbStack.empty())
		cbStack.back()->IR_OnDeactivate();

	cbStack.push_back(p);
	cbStack.back()->IR_OnActivate();
}

void CInput::iRelease(IInputReceiver *p)
{
	ResetPressedState();

	if (cbStack.empty()) return;
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
	{
		// we are not topmost receiver, so remove the nearest one
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

void CInput::OnAppActivate()
{
	if (CurrentIR())
		CurrentIR()->IR_OnActivate();

	ResetPressedState();
	// set mouse to center screen
	RECT windowRect;
	Device.GetXrWindowRect(windowRect);
	cachedWindowRect = windowRect;

	SetCursorPos(windowRect.right - (LONG)Device.fWidth_2, windowRect.bottom - (LONG)Device.fHeight_2);
	XInputEnable(TRUE);

	if (bShouldLockMouse)
		LockMouse();
}

void CInput::OnAppDeactivate()
{
	if (CurrentIR())
		CurrentIR()->IR_OnDeactivate();

	ResetPressedState();
	ClipCursor(NULL);
	XInputEnable(FALSE);
}

void CInput::LockMouse()
{
	bShouldLockMouse = true;
	RECT windowRect;
	Device.GetXrWindowRect(windowRect);

	if (!bAllowBorderAccess)
	{
		// make offset for 15 px from window border, to prevent accidently resizing
		windowRect.left += 15;
		windowRect.right -= 15;
		windowRect.top += 55;
		windowRect.bottom -= 25;
	}

	ClipCursor(&windowRect);
	PlatformUtils.ShowCursor(FALSE);
}

void CInput::UnlockMouse()
{
	ClipCursor(NULL);
	bShouldLockMouse = false;
	PlatformUtils.ShowCursor(TRUE);
}

void CInput::ResetPressedState()
{
	if (!cbStack.empty())
	{
		// invoke necessery Release handlers
		IInputReceiver* receiver = cbStack.back();
		for (u8 i = 0; i < 255; ++i)
		{
			if (pressedKeys[i])
			{
				pressedKeys[i] = false;
				receiver->IR_OnKeyboardRelease(i);
			}
		}
	}
	else
	{
		RtlZeroMemory(&pressedKeys[0], sizeof(pressedKeys));
	}
}


void CInput::OnFrame()
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

	// check for gamepad presense
	if ((Device.dwFrame % 60) == 0) // per 60 frames
	{
		CheckGamepad();
	}

	// get gamepad inputs (if presented)
	if (bGamepadConnected)
	{
		XINPUT_STATE GamepadState;
		if ((XInputGetState(gamepadUserIndex, &GamepadState)) == ERROR_SUCCESS)
		{
			if (gamepadLastPacketId != GamepadState.dwPacketNumber)
			{
				XINPUT_GAMEPAD& GamepadData = GamepadState.Gamepad;
				gamepadLastPacketId = GamepadState.dwPacketNumber;

				// check for buttons
				auto CheckForGamepadButtonLambda = [this, &GamepadData](u16 xInputButtonMask, u8 VkKey)
				{
					if (GamepadData.wButtons & xInputButtonMask)
					{
						if (!pressedKeys[VkKey])
						{
							pressedKeys[VkKey] = true;
							cbStack.back()->IR_OnKeyboardPress(VkKey);
						}
					}
					else
					{
						if (pressedKeys[VkKey])
						{
							pressedKeys[VkKey] = false;
							cbStack.back()->IR_OnKeyboardRelease(VkKey);
						}
					}
				};

				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_DPAD_UP,			VK_GAMEPAD_DPAD_UP);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_DPAD_DOWN,		VK_GAMEPAD_DPAD_DOWN);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_DPAD_LEFT,		VK_GAMEPAD_DPAD_LEFT);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_DPAD_RIGHT,		VK_GAMEPAD_DPAD_RIGHT);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_START,			VK_GAMEPAD_MENU);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_BACK,			VK_GAMEPAD_VIEW);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_LEFT_THUMB,		VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_RIGHT_THUMB,		VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_LEFT_SHOULDER,	VK_GAMEPAD_LEFT_SHOULDER);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_RIGHT_SHOULDER,	VK_GAMEPAD_RIGHT_SHOULDER);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_A,				VK_GAMEPAD_A);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_B,				VK_GAMEPAD_B);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_X,				VK_GAMEPAD_X);
				CheckForGamepadButtonLambda(XINPUT_GAMEPAD_Y,				VK_GAMEPAD_Y);

				auto ProcessValueButtonLambda = [this](float CurrentValue, float PreviousValue, bool bNegative, u8 VkKey)
				{
					float Comparer = bNegative ? -0.9f : 0.9f;
					if (bNegative)
					{
						if (CurrentValue < Comparer && PreviousValue > Comparer)
						{
							cbStack.back()->IR_OnKeyboardPress(VkKey);
						}
						else if (CurrentValue > Comparer && PreviousValue < Comparer)
						{
							cbStack.back()->IR_OnKeyboardRelease(VkKey);
						}
					}
					else
					{
						if (CurrentValue > Comparer && PreviousValue < Comparer)
						{
							cbStack.back()->IR_OnKeyboardPress(VkKey);
						}
						else if (CurrentValue < Comparer && PreviousValue > Comparer)
						{
							cbStack.back()->IR_OnKeyboardRelease(VkKey);
						}
					}
				};

				// check for triggers and thumbsticks
				auto ProcessTriggerInputLambda = [this, &ProcessValueButtonLambda](GamepadTriggerType type, u8 VkKey, BYTE InTriggerInput, float& previousTriggerValue)
				{
					float fltTriggerValue = (float)InTriggerInput / 255.f;
					if (!float_equal(fltTriggerValue, previousTriggerValue))
					{
						ProcessValueButtonLambda(fltTriggerValue, previousTriggerValue, false, VkKey);

						previousTriggerValue = fltTriggerValue;
						cbStack.back()->IR_OnTriggerPressed(type, previousTriggerValue);
					}
				};

				ProcessTriggerInputLambda(GamepadTriggerType::Left,  VK_GAMEPAD_LEFT_TRIGGER,  GamepadData.bLeftTrigger,  leftTrigger);
				ProcessTriggerInputLambda(GamepadTriggerType::Right, VK_GAMEPAD_RIGHT_TRIGGER, GamepadData.bRightTrigger, rightTrigger);

				auto ProcessThumbstickInputLambda = [this, &ProcessValueButtonLambda](GamepadThumbstickType thumbstickType, Fvector2& thumbstickValue, short ThumbX, short ThumbY, short gamepadDeadZone, u8 VkKeyLeft, u8 VkKeyRight, u8 VkKeyUp, u8 VkKeyDown)
				{
					float ThumbstickXValue = 0.0f;
					float ThumbstickYValue = 0.0f;
					if (ThumbX > gamepadDeadZone || ThumbX < (-gamepadDeadZone))
					{
						ThumbstickXValue = (float)ThumbX / 32767.f;
					}
					if (ThumbY > gamepadDeadZone || ThumbY < (-gamepadDeadZone))
					{
						ThumbstickYValue = (float)ThumbY / 32767.f;
						ThumbstickYValue = -ThumbstickYValue;
					}

					if (!float_equal(thumbstickValue.x, ThumbstickXValue) || !float_equal(thumbstickValue.y, ThumbstickYValue))
					{
						ProcessValueButtonLambda(ThumbstickXValue, thumbstickValue.x, true,     VkKeyLeft);
						ProcessValueButtonLambda(ThumbstickXValue, thumbstickValue.x, false,    VkKeyRight);
						ProcessValueButtonLambda(ThumbstickYValue, thumbstickValue.y, true,		VkKeyUp);
						ProcessValueButtonLambda(ThumbstickYValue, thumbstickValue.y, false,	VkKeyDown);

						thumbstickValue.x = ThumbstickXValue;
						thumbstickValue.y = ThumbstickYValue;
						cbStack.back()->IR_OnThumbstickChanged(thumbstickType, thumbstickValue);
					}
				};

				ProcessThumbstickInputLambda(GamepadThumbstickType::Left, leftThumbstick, GamepadData.sThumbLX, GamepadData.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
					VK_GAMEPAD_LEFT_THUMBSTICK_LEFT, VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT, VK_GAMEPAD_LEFT_THUMBSTICK_UP, VK_GAMEPAD_LEFT_THUMBSTICK_DOWN);

				ProcessThumbstickInputLambda(GamepadThumbstickType::Right, rightThumbstick, GamepadData.sThumbRX, GamepadData.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
					VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT, VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT, VK_GAMEPAD_RIGHT_THUMBSTICK_UP, VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN);
			}
		}

		if (bVibrationStarted)
		{
			if (stop_vibration_time < RDEVICE.fTimeGlobal)
			{
				bVibrationStarted = false;
				XINPUT_VIBRATION vibrationInputData;
				vibrationInputData.wLeftMotorSpeed =  0;
				vibrationInputData.wRightMotorSpeed = 0;

				XInputSetState(gamepadUserIndex, &vibrationInputData);
			}
		}
	}
}

IInputReceiver*	 CInput::CurrentIR()
{
	return !cbStack.empty() ? cbStack.back() : nullptr;
}

void CInput::CheckGamepad()
{
	XINPUT_CAPABILITIES GamepadState;
	
	for (u32 i = 0; i < XUSER_MAX_COUNT; i++)
	{
		if (XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &GamepadState) == ERROR_SUCCESS)
		{
			gamepadUserIndex = i;
			bGamepadConnected = true;

			if (GamepadState.Vibration.wLeftMotorSpeed && GamepadState.Vibration.wRightMotorSpeed)
			{
				bIsVibrationSupported = true;
			}

			return;
		}
	}

	bIsVibrationSupported = false;
	bGamepadConnected = false;
}

bool CInput::IsGamepadPresented() const
{
	return bGamepadConnected;
}

void CInput::SetAllowAccessToBorders(bool bAccessToBorders)
{
	bAllowBorderAccess = bAccessToBorders;
	//LockMouse();
}

void CInput::OnScreenResolutionChanged(void)
{
	RECT windowRect;
	Device.GetXrWindowRect(windowRect);
	cachedWindowRect = windowRect;
}

void  CInput::feedback(u16 s1, u16 s2, float time)
{
	stop_vibration_time = RDEVICE.fTimeGlobal + time;
	bVibrationStarted = true;

	XINPUT_VIBRATION vibrationInputData;
	vibrationInputData.wLeftMotorSpeed = s1;
	vibrationInputData.wRightMotorSpeed = s2;
	
	XInputSetState(gamepadUserIndex, &vibrationInputData);
}
