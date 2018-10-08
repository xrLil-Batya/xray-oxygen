#include "files_list.hpp"
#include "EditorLevel.h"
#include "../../xrEngine/Environment.h"
#include "../../xrEngine/Render.h"


void CEditorLevel::Load_GameSpecific_CFORM(CDB::TRI* T, u32 count)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CEditorLevel::OnRender(void)
{
	Render->Calculate();
	Render->Render();
}

void CEditorLevel::SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor)
{
	Environment().SetGameTime(GameTime, fTimeFactor);
}

shared_str CEditorLevel::name() const
{
	return "Sandbox";
}

shared_str CEditorLevel::name_translated() const
{
	return "Sandbox";
}

BOOL CEditorLevel::net_Start(LPCSTR op_server, LPCSTR op_client)
{
	return TRUE;
}

void CEditorLevel::net_Update()
{
	// We can make something here
	
}

void CEditorLevel::OnEvent(EVENT E, u64 P1, u64 P2)
{

}
