#pragma once

class ENGINE_API xrDiscordPresense
{
public:
	enum class StatusId
	{
		Menu,
		In_Game,
		Zaton,
		Upiter,
		Pripyat
	};

public:

	void Initialize();
	void Shutdown();

	void SetStatus(StatusId status);

	~xrDiscordPresense();

private:
	bool bInitialize = false;
};

extern ENGINE_API xrDiscordPresense g_discord;