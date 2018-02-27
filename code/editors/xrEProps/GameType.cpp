#include "pch.hpp"
#include "GameType.h"
#include "xrServerEntities/gametype_chooser.h"

namespace XRay
{
namespace ECore
{
namespace Props
{
bool GameType::Run(pcstr title, GameTypeChooser* data)
{
    gameTypes = data;

    checkSingle->Checked = gameTypes->MatchType(eGameIDSingle);

    return ShowDialog() == Windows::Forms::DialogResult::OK;
}

System::Void GameType::buttonOk_Click(System::Object^ sender, System::EventArgs^ e)
{
    gameTypes->m_GameType.zero();
    gameTypes->m_GameType.set(eGameIDSingle, checkSingle->Checked);
}

} // namespace Props
} // namespace ECore
} // namespace XRay
