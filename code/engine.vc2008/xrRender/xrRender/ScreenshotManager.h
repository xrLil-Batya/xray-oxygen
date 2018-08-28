#pragma once

const int GAMESAVE_IMAGE_SIZE = 128;

class CScreenshotManager
{
public:
	void		MakeScreenshot				(IRender_interface::ScreenshotMode mode, LPCSTR name);

private:
	void		ProcessImage				(u32* pData, u32 size, bool bGammaCorrection);

	// Implementation
	ID3DBlob*	MakeScreenshotNormal		(u32 fmt);
	ID3DBlob*	MakeScreenshotForSavedGame	();
	ID3DBlob*	MakeScreenshotForCubeMap	(u32 fmt);
	ID3DBlob*	MakeScreenshotForLevelMap	(u32 fmt);
};