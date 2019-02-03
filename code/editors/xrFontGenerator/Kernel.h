/*
  ******************OXYGEN DEV TEAM (C) 2019**************************
  ******************    Author: Phantom1020 (samber13 - это приложение вообще не делал и не помогал, может подтвердить)    **************************
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
constexpr unsigned int TOTAL_ANSCII = 96 + 63 + 1;

namespace XRay
{
	// @ Contains information about glyph or symbol in texture
	struct data_symbol
	{
		size_t x[1], y[1];
		size_t x_off, y_off;
		int advance;
	};
	
	struct PathList
	{
		xr_string PathName;
		xr_string FileName;

		xr_string PathOutName;
		xr_string FileOutName;
	};

	struct ConvInfo
	{
		FT_Library lib;
		FT_Face    face;

		unsigned int   TexWid = 1; // @ Не ставить это в ноль, ибо дальше умножаем на два
		unsigned int   TexHeig = 0;
		unsigned int   FontHeig = 0;
					   
		unsigned int   PenX = 0;
		unsigned int   PenY = 0;
		unsigned char* Pixels;

		bool bHaveTexconv; // @ Для проверки, имеет ли юзер в папке texconv или нет;
	};

	data_symbol info[TOTAL_ANSCII];
	data_symbol info_copy = { 0 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	inline void HeaderMessage() noexcept
	{  
		std::cout << "************************************************" << std::endl;
		std::cout << "**********      ";
		SetConsoleTextAttribute(hConsole, 27);
		std::cout << "OXYGEN DEV TEAM(C)";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << " 	  ******" << std::endl;
		std::cout << "********** ";
		SetConsoleTextAttribute(hConsole, 27);
		std::cout << "TEXTURE GENERATOR OF TTF FONTS";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << " ******" << std::endl;
		std::cout << "********** ";
		SetConsoleTextAttribute(hConsole, 27);
		std::cout << "POWERED BY ";
		SetConsoleTextAttribute(hConsole, 18);
		std::cout << "FREETYPE";
		SetConsoleTextAttribute(hConsole, 27);
		std::cout << ",";
		SetConsoleTextAttribute(hConsole, 29);
		std::cout << " STBI lib";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << " ******" << std::endl;
		std::cout << "************************************************" << std::endl; 
	}

	void ManageCreationFile(void);
	void CreateFolder(void);

	// @ Creates already with _size_800, _size_1024, _size_1600
	void InitFont		(); 
	void CreateGSCFonts	();
	void CheckTexConv	();
	void InitFreeType	();
	void ReleaseFreeType();
}