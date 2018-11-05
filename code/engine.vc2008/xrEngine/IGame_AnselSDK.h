#pragma once

class ENGINE_API CGameAnsel
{
public:
	CGameAnsel();
	virtual bool Load() { return false; };

public:
	bool isActive;
	float offsetX;
	float offsetY;
	float viewportNear;

};

ENGINE_API extern CGameAnsel* pGameAnsel;