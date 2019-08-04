#pragma once

class	ENGINE_API				IInputReceiver;

class ENGINE_API CInput : public pureFrame, public pureAppActivate, public pureAppDeactivate, public pureScreenResolutionChanged
{
public:
	CInput();
	~CInput();

	void						iCapture					( IInputReceiver *pc );
	void						iRelease					( IInputReceiver *pc );
	BOOL						iGetAsyncKeyState			( u8 dik );
	BOOL						iGetAsyncBtnState			( u8 btn );
	void						iGetLastMouseDelta			( Ivector2& p );

	virtual void				OnFrame						() override;
	virtual void				OnAppActivate				() override;
	virtual void				OnAppDeactivate				() override;

	void						LockMouse();
	void						UnlockMouse();


	void						ProcessInput(LPARAM hRawInput);
	IInputReceiver*				CurrentIR					();

	void						CheckGamepad();
	bool						IsGamepadPresented() const;

	void						SetAllowAccessToBorders(bool bAccessToBorders);

	virtual void				OnScreenResolutionChanged(void) override;

public:
	bool						get_VK_name(u8 dik, LPSTR dest, int dest_sz);

	void						feedback(u16 s1, u16 s2, float time);

private:

	void ResetPressedState();

	RAWINPUTDEVICE		rawDevices[2];
	u32					dwCurTime;

	// buffers for input
	u8 inputData[1024];

	// last input data
	Ivector2 deltaMouse;
	bool pressedKeys[0xFF];
	// for gamepads
	Fvector2 leftThumbstick;
	Fvector2 rightThumbstick;
	float    leftTrigger;
	float    rightTrigger;

	bool bAllowBorderAccess;
	bool bCursorShowed;
	bool bShouldLockMouse = true;

	bool bGamepadConnected;
	bool bIsVibrationSupported;
	bool bVibrationStarted;
	u32  gamepadUserIndex;
	u32  gamepadLastPacketId;
	RECT cachedWindowRect;

	xr_vector<IInputReceiver*>	cbStack;
};

extern ENGINE_API CInput *		pInput;