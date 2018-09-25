// File:		UIMessagesWindow.h
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World
#pragma once
#include "../xrUICore/UIWindow.h"

struct GAME_NEWS_DATA;
class  CUIGameLog;

class CUIMessagesWindow : public CUIWindow 
{
public:
						CUIMessagesWindow				();
	virtual				~CUIMessagesWindow				();

	void				AddIconedPdaMessage				(GAME_NEWS_DATA* news);

	void				AddLogMessage					(const shared_str& msg);
	void				PendingMode						(bool const is_in_pending_mode);
	virtual void		Show							(bool show);


protected:
	virtual void		Init(float x, float y, float width, float height);

	bool				m_in_pending_mode;
	CUIGameLog*			m_pGameLog;
	Frect				m_pending_chat_log_rect;
	Frect				m_inprogress_chat_log_rect;
};