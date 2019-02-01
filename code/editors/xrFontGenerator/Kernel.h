/*
  ******************OXYGEN DEV TEAM (C) 2019**************************
  ******************    Author: Samber13    **************************
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
 

#define TOTAL_ANSCII 127+63+2

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
		std::cout << " DirectxTK";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << " ******" << std::endl;
		std::cout << "************************************************" << std::endl; 
	}

	void ManageCreationFile(void);
	void CreateFolder(void);

	// @ Creates already with _size_800, _size_1024, _size_1600
	void InitFont(void); 


	void CreateTextureResolution(void);
	void ChangeFaceSize(void);
	void CreateConfigs(void);
	void CheckLanguages(void);

	void InitFreeType(void);

	// @ FREE
	void ReleaseDirectXData(void);
	void ReleaseFreeType(void);


	// @ For Console Handling 
	void ReDrawInfo(void);

	FT_Library lib;
	FT_Face face;

	int tex_width = 1;
	int tex_height = 0;
	int font_height = 0;
	std::string output = "";
	std::string path_to_new_folder = "";
	std::string path_to_loaded_file = "";
}