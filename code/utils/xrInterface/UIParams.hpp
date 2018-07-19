#pragma once

struct UIParams
{
	bool isShutDown;

	UIParams()
	{
		isShutDown = false;
	}
};

extern XRLCUTIL_API UIParams* pUIParams;