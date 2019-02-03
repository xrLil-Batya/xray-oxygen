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
constexpr int TOTAL_ANSCII = 96 + 63 + 1;

namespace Oxy
{
	// @ Contains information about glyph or symbol in texture
	struct data_symbol
	{
		int x0, y0, x1, y1;
		int x_off, y_off;
		int advance;
	};

	data_symbol info[TOTAL_ANSCII];
	data_symbol info_copy = { 0 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	inline void HeaderMessage(void) 
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
	void InitFont(void); 

	void CreateGSCFonts(void);
	void CheckTexConv(void);

	void InitFreeType(void);

	// @ FREE
	void ReleaseFreeType(void);

	FT_Library lib;
	FT_Face face;

	
	int tex_width = 1; // @ Не ставить это в ноль, ибо дальше умножаем на два
	int tex_height = 0;
	int font_height = 0;
	std::string output = "";
	std::string path_to_new_folder = "";
	std::string path_to_loaded_file = "";
	std::string filename = "";
	int pen_x = 0;
	int pen_y = 0;
	unsigned char* pixels;
	bool have_we_texconv = false; // @ Для проверки, имеет ли юзер в папке texconv или нет;
}