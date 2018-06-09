/******************************************************************************************************************
***** Authors: Lord
***** Date of creation: 03.05.2018
***** Description: File of impelmentation Render of DockPanel's element for integration in Main Form in Level Editor
***** Copyright: GSC, OxyGen Team 2018 (C)
******************************************************************************************************************/
#pragma once

namespace OxySDK
{
	/* Reductions:
			RV -> Render_View
			MItem -> MenuItem
			ZExtent -> Zoom Extent
			FView -> Front View
			BView -> Back View
			LView -> Left View
			RView -> Right View
			TView -> Top View
			BView -> Bottom View
			RView -> Reset View
			CamLookP -> Perspective, Camera moves very fast but it's hard to move for some detail position like in CamLookF
			CamLookA -> Aligment, Camera's View centered at center scene and Camera's view is fixed you can only rotate camera, but can't move to forward and another directions
			CamLookF -> First Look Camera it's like your camera moves very slow, but realistic 
	*/
 
	public ref class RenderView : public WeifenLuo::WinFormsUI::DockContent
	{
	
	private:
		// Context Menu
		System::Windows::Forms::ContextMenu RV_ContextMenu;
		System::Windows::Forms::MenuItem RV_MItem_Undo;
		System::Windows::Forms::MenuItem RV_MItem_Redo;
		System::Windows::Forms::MenuItem RV_MItem_Cursor;
		System::Windows::Forms::MenuItem RV_MItem_Add;
		System::Windows::Forms::MenuItem RV_MItem_Move;
		System::Windows::Forms::MenuItem RV_MItem_Rotate;
		System::Windows::Forms::MenuItem RV_MItem_UniformScale;
		System::Windows::Forms::MenuItem RV_MItem_AxisX;
		System::Windows::Forms::MenuItem RV_MItem_AxisY;
		System::Windows::Forms::MenuItem RV_MItem_AxisZ;
		System::Windows::Forms::MenuItem RV_MItem_AxisZX;
		System::Windows::Forms::MenuItem RV_MItem_CSToggle;
		System::Windows::Forms::MenuItem RV_MItem_NonUniformScale;
		System::Windows::Forms::MenuItem RV_MItem_GridSnap;
		System::Windows::Forms::MenuItem RV_MItem_ObjectSnap;

		//@ Moving Snap To Object Toggle
		System::Windows::Forms::MenuItem RV_MItem_MSTOT;
		System::Windows::Forms::MenuItem RV_MItem_NormalAlig;
		System::Windows::Forms::MenuItem RV_MItem_VertexSnap;
		System::Windows::Forms::MenuItem RV_MItem_AngleSnap;
		System::Windows::Forms::MenuItem RV_MItem_MovingSnap;
		System::Windows::Forms::MenuItem RV_MItem_ZExtent;
		System::Windows::Forms::MenuItem RV_MItem_ZExtents;
		System::Windows::Forms::MenuItem RV_MItem_FView;
		System::Windows::Forms::MenuItem RV_MItem_BView;
		System::Windows::Forms::MenuItem RV_MItem_LView;
		System::Windows::Forms::MenuItem RV_MItem_RView;
		System::Windows::Forms::MenuItem RV_MItem_TView;
		System::Windows::Forms::MenuItem RV_MItem_BView;
		System::Windows::Forms::MenuItem RV_MItem_RView;

		System::Windows::Forms::MenuItem RV_MItem_CamLookP;
		System::Windows::Forms::MenuItem RV_MItem_CamLookA;
		System::Windows::Forms::MenuItem RV_MItem_CamLookF;
		// Context Menu	

	};
}