// IInputReceiver.h: interface for the IInputReceiver class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

enum class GamepadThumbstickType
{
	Left,
	Right
};

enum class GamepadTriggerType
{
	Left,
	Right
};

class ENGINE_API	IInputReceiver
{
public:
	void			IR_GetLastMouseDelta			(Ivector2& p);
	void			IR_GetMousePosScreen			(Ivector2& p);
	void			IR_GetMousePosReal				(HWND hwnd, Ivector2 &p);
	void			IR_GetMousePosReal				(Ivector2 &p);
	void			IR_GetMousePosIndependent		(Fvector2 &f);
	void			IR_GetMousePosIndependentCrop	(Fvector2 &f);
	BOOL			IR_GetKeyState (u8 dik);
	void			IR_Capture						();
	void			IR_Release						();

	virtual void	IR_OnDeactivate					();
	virtual void	IR_OnActivate					();

	virtual void	IR_OnMouseWheel					(int direction)	{};
	virtual void	IR_OnMouseMove					(int x, int y)	{};

	virtual void	IR_OnKeyboardPress				(u8 dik)		{};
	virtual void	IR_OnKeyboardRelease			(u8 dik)		{};
	virtual void	IR_OnKeyboardHold				(u8 dik)		{};

	// gamepad
	virtual void	IR_OnThumbstickChanged (GamepadThumbstickType type, const Fvector2& position) {};
	virtual void    IR_OnTriggerPressed(GamepadTriggerType type, float value) {};
};

ENGINE_API extern float			psMouseSens;
ENGINE_API extern float			psMouseSensScale;
ENGINE_API extern Flags32		psMouseInvert;
