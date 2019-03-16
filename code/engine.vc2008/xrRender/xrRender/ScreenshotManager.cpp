#include "stdafx.h"
#include "ScreenshotManager.h"

// D3D-compliant image formats (see D3DXIMAGE_FILEFORMAT and D3DX11_IMAGE_FILE_FORMAT)
enum
{
	SS_BMP = 0,
	SS_PNG = 3,
	SS_DDS = 4,
};

void CScreenshotManager::MakeScreenshot(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if (!Device.b_is_Ready)
		return;

	ID3DBlob* pSaved = nullptr;
	string64 fileName;
	string_path filePath;

	// Choose file format
	LPCSTR ext = "png";
	u32 fmt = SS_PNG;
	if (strstr(Core.Params, "-ss_bmp"))
	{
		ext = "bmp";
		fmt = SS_BMP;
	}

	switch (mode)
	{
	case IRender_interface::SM_NORMAL:
	{
		pSaved = MakeScreenshotNormal(fmt);

		if (name && xr_strlen(name) > 0)
			xr_sprintf(fileName, "%s.%s", name, ext);
		else
		{
			// Make filename from date-time and level name
			string32 buf;
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);
			xr_sprintf(buf, "%02hu-%02hu-%hu %02hu-%02hu-%02hu", localTime.wDay, localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute, localTime.wSecond);
			xr_sprintf(fileName, sizeof(fileName), "%s (%s).%s", buf, (g_pGameLevel) ? g_pGameLevel->name_translated().c_str() : "Main Menu", ext);
		}
		FS.update_path(filePath, "$screenshots$", fileName);
	}
	break;
	case IRender_interface::SM_FOR_GAMESAVE:
	{
		pSaved = MakeScreenshotForSavedGame();
		xr_strcpy(filePath, sizeof(filePath), name);
	}
	break;
	case IRender_interface::SM_FOR_LEVELMAP:
	{
		pSaved = MakeScreenshotForLevelMap(fmt);
		xr_strconcat( fileName, name, ext);
		FS.update_path(filePath, "$screenshots$", fileName);
	}
	break;
	case IRender_interface::SM_FOR_CUBEMAP:
	{
		pSaved = MakeScreenshotForCubeMap(fmt);
		xr_strconcat( fileName, name, ext);
		FS.update_path(filePath, "$screenshots$", fileName);
	}
	break;
	}

	IWriter* fs = FS.w_open(filePath);
	R_ASSERT(fs);
	fs->w(pSaved->GetBufferPointer(), pSaved->GetBufferSize());
	FS.w_close(fs);
	_RELEASE(pSaved);
}
