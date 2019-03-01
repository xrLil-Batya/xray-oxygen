/*
  ******************OXYGEN DEV TEAM (C) 2019**************************
  ******************    Author: Phantom1020  **************************
  ******************    CLI/WinForms: ForserX  **************************
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

// @ Да за хардкоржено, отводим место для Английского и Русского, а также для системных символов (цифры, прочее)
constexpr unsigned int TOTAL_ANSCII = 96 + 65 + 1;

namespace XRay
{
	// @ Contains information about glyph or symbol in texture
	struct data_symbol
	{
		int advance;
		size_t x_off, y_off;
		size_t x[2], y[2];
	};

	struct PathList
	{
		u32 FontSize;

		xr_string PathName;
		xr_string FileName;

		xr_string PathOutName;
		xr_string FileOutName;
	};

	struct ConvInfo
	{
		FT_Library lib;
		FT_Face    face;

		// @ Не ставить это в ноль, ибо дальше умножаем на два
		// @ Размер текстуры
		unsigned int   TexWid = 1; 
		unsigned int   TexHeig = 0;

		// @ Здесь всё очевидно
		unsigned int   FontHeig = 0;
					   
		// @ Просчитываемая позиция для последующий символов, которые будут добавляться на этапе растеризации
		int   PenX = 0;
		int   PenY = 0;

		// @ Промежуточный буфер для хранения пикселей, как ни странно
		unsigned char* Pixels;

		// @ Для проверки, имеет ли юзер в папке texconv или нет;
		bool bHaveTexconv; 
	};
	
	extern HANDLE hConsole;

	class CFontGen
	{
		data_symbol info[TOTAL_ANSCII];
		data_symbol info_copy = { 0 }; 
		ConvInfo ConverterInfo;
	public:
		static bool bSucFile;
		static bool bSucDir;
		PathList PathSystem;

	public:
		CFontGen();
		~CFontGen() = default;

		void ParseFont(int index, int max_value);
		void ManageCreationFile(void);
		void CreateFolder(void);

		// @ Creates already with _size_800, _size_1024, _size_1600
		void InitFont();
		void CreateGSCFonts();
		void CheckTexConv();
		void InitFreeType();
		void ReleaseFreeType();
	};
}