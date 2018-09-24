// File:        UIInteractiveBackground.h
// Description: template class designed for UI controls to represent their state;
//              there are 4 states: Enabled, Disabled, Hightlighted and Touched.
//              As a rule you can use one of 3 background types:
//              Normal Texture, String Texture, Frame Texture (CUIStatic, CUIFrameLineWnd, CUIFrameWindow)
// Created:     29.12.2004
// Author:      Serhiy 0. Vynnychenko
// Mial:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//
#pragma once
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"

enum IBState{
	S_Enabled		=0,
	S_Disabled,
	S_Highlighted,
	S_Touched,
	S_Current,
	S_Total
};

template <class T>
class UI_API CUIInteractiveBackground : public CUIWindow
{
public:
	CUIInteractiveBackground();
	virtual ~CUIInteractiveBackground() {};

			void InitIB				(Fvector2 pos, Fvector2 size);
			void InitIB				(LPCSTR texture_e, Fvector2 pos, Fvector2 size);
			T*	 Get				(IBState state){return m_states[state];};

			void InitState			(IBState state, LPCSTR texture);
			void SetCurrentState	(IBState state);

	virtual void Draw				();
	virtual void SetWidth			(float width);
	virtual void SetHeight			(float heigth);

protected:
	T*			m_states [S_Total];
};

typedef CUIInteractiveBackground<CUIFrameLineWnd> CUI_IB_FrameLineWnd;
