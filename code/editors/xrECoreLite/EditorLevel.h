#pragma once
#include "../../xrEngine/stdafx.h"
#include "../../xrEngine/IGame_Level.h"

class CEditorLevel : public IGame_Level
{
	using inherited = IGame_Level;
public:
	virtual void Load_GameSpecific_CFORM(CDB::TRI* T, u32 count) override;
	virtual void OnRender(void) override;
	virtual void SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor) override;
	virtual shared_str name() const override;
	virtual shared_str name_translated() const override;
	virtual BOOL net_Start(LPCSTR op_server, LPCSTR op_client) override;
	virtual void net_Update() override;
	virtual void OnEvent(EVENT E, u64 P1, u64 P2) override;
};