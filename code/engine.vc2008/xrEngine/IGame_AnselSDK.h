#pragma once

class ENGINE_API IGameAnsel
{
public:
	IGameAnsel();
	virtual ~IGameAnsel() = default;
	virtual bool Load() { return false; };

public:
	static bool IsActive();

	float offsetX;
	float offsetY;
	float viewportNear;

protected:
	bool bIsActive;
};

ENGINE_API extern IGameAnsel* pGameAnsel;