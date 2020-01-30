#pragma once

#include "../xrUICore/UIDialogWnd.h"
#include "../xrUICore/UIStatic.h"
#include "../xrUICore/UIButton.h"
#include "../xrUICore/UIEditBox.h"
#include "../xrUICore/UIFrameWindow.h"
#include "../PhraseDialogDefs.h"

class CActor;
class CInventoryOwner;
class CPhraseDialogManager;
class CUITalkDialogWnd;

///////////////////////////////////////
//
///////////////////////////////////////
class CUITalkWnd : public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	ref_sound m_sound;
	void PlaySnd(LPCSTR text);
	void StopSnd();

public:
	CUITalkWnd();
	virtual ~CUITalkWnd();

	IC bool playing_sound() { return !!m_sound._feedback(); }
	bool GetInitState() const { return m_bInitState; }
	IC CInventoryOwner* OthersInvOwner() const { return m_pOthersInvOwner; }

	void InitTalkWnd();

	virtual bool StopAnyMove() { return true; }
	virtual void SendMessageToWnd(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

	virtual void Draw();
	virtual void Update();

	virtual void Show(bool status);

	void Stop();					//deffered
	void StopTalk();

	void UpdateQuestions();
	void NeedUpdateQuestions();

	//инициализации начального диалога собеседника
	void InitOthersStartDialog();
	virtual bool OnKeyboardAction(u8 dik, EUIMessages keyboard_action);
	void SwitchToTrade();
	void SwitchToUpgrade();
	void AddIconedMessage(LPCSTR caption, LPCSTR text, LPCSTR texture_name, LPCSTR templ_name);

protected:
	//диалог
	void InitTalkDialog();
	void AskQuestion();

	void SayPhrase(const shared_str& phrase_id);

	// Функции добавления строк в листы вопросов и ответов
public:
	void AddQuestion(const shared_str& text, const shared_str& id, int number, bool b_finalizer);
	void AddAnswer(const shared_str& text, LPCSTR SpeakerName);
	bool b_disable_break;
protected:
	CUITalkDialogWnd* UITalkDialogWnd;

	CActor*	m_pActor;
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;

	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	bool m_bNeedToUpdateQuestions;
	bool m_bInitState;

	//текущий диалог, если NULL, то переходим в режим выбора темы
	DIALOG_SHARED_PTR m_pCurrentDialog;
	bool TopicMode();
	void ToTopicMode();
};