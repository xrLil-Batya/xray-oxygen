////////////////////////////////////////////////////
// Author: Viнt@rь
// Task  : Static UI component that rendering 3d visual in self
////////////////////////////////////////////////////
// Specital for X-Ray Oxygen Project | 02.02.2018 //
////////////////////////////////////////////////////
#pragma once
#include "uiwindow.h"

class CGameObject;

class UI_API CUI3dStatic : public CUIWindow
{
public:
	CUI3dStatic();
	virtual ~CUI3dStatic();

	void SetRotate(float x, float y, float z) { m_x_angle = x; m_y_angle = y; m_z_angle = z; }

	void SetGameObject(CObject* pItem);

	//прорисовка окна
	virtual void Draw();

protected:
	float m_x_angle, m_y_angle, m_z_angle;

	//перевод из координат экрана в координаты той плоскости
	//где находиться объект
	void FromScreenToItem(int x_screen, int y_screen, float& x_item, float& y_item);

	CObject* m_pCurrentItem;
};