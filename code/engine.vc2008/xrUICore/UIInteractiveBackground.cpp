#include "stdafx.h"
#include "UIInteractiveBackground.h"

template <class T>
CUIInteractiveBackground<T>::CUIInteractiveBackground()
{
	std::memset(m_states, 0, S_Total * sizeof(T*));
}


template <class T>
void CUIInteractiveBackground<T>::InitIB(Fvector2 pos, Fvector2 size)
{
	CUIWindow::SetWndPos(pos);
	CUIWindow::SetWndSize(size);
}

template <class T>
void CUIInteractiveBackground<T>::InitIB(LPCSTR texture, Fvector2 pos, Fvector2 size)
{
	CUIWindow::SetWndPos(pos);
	CUIWindow::SetWndSize(size);

	InitState(S_Enabled, texture);
}

template <class T>
void CUIInteractiveBackground<T>::InitState(IBState state, LPCSTR texture)
{
	Fvector2 size = GetWndSize();

	if (!m_states[state])
	{
		m_states[state] = xr_new<T>();
		m_states[state]->SetAutoDelete(true);
		AttachChild(m_states[state]);
	}

	m_states[state]->InitTexture(texture);
	m_states[state]->SetWndPos(Fvector2().set(0, 0));
	m_states[state]->SetWndSize(size);

	SetCurrentState(state);
}

template <class T>
void CUIInteractiveBackground<T>::SetCurrentState(IBState state)
{
	m_states[S_Current] = m_states[state];
	if (!m_states[S_Current])
		m_states[S_Current] = m_states[S_Enabled];
}

template <class T>
void CUIInteractiveBackground<T>::Draw()
{
	if (m_states[S_Current])
		m_states[S_Current]->Draw();
}

template <class T>
void CUIInteractiveBackground<T>::SetWidth(float width)
{
	for (int i = 0; i<S_Total; ++i)
		if (m_states[i])
			m_states[i]->SetWidth(width);
}

template <class T>
void CUIInteractiveBackground<T>::SetHeight(float height)
{
	for (int i = 0; i<S_Total; ++i)
		if (m_states[i])
			m_states[i]->SetHeight(height);
}
