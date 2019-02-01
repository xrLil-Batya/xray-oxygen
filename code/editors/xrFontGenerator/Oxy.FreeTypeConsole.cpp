// Oxy.FreeTypeConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Kernel.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <combaseapi.h>
#include <fstream>
 
 
#define PAUSE_AND_CLEAR std::system("pause"); std::system("cls");
#define CLEAR_WINDOW std::system("cls");

constexpr int DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
constexpr int ERROR_COLOR = 28;
constexpr int OK_COLOR = 26;
constexpr int COLOR_DEFAULT = 27;
constexpr int PAUSE_TIME = 1000;

// @ If method ManageCreationFile is successful processing
bool suc_file = false;
bool suc_dir = false;
// @ ReDrawing stuff
bool can_draw_part1 = false; // About Directx and FreeType initialization
bool can_draw_part2 = false; // About Loading Font and Creating output folder
bool can_draw_part3 = false; // About Folder
bool can_draw_part4 = false; // About Supports which langs
bool can_draw_part5 = false; // About size (and final algorithm)
bool supported_english = false;
bool supported_russian = false;
bool supported_japan_h = false;
bool supported_japan_k = false;
bool supported_japan = false;
bool supported_chineese = false;
int size = 0;


void Oxy::InitFreeType(void)
{
	FT_Error err = FT_Init_FreeType(&lib);
	std::cout << "Initializing FreeType: ";
	if (err)
	{
		Sleep(PAUSE_TIME);
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "Failed!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}
	else
	{
		Sleep(PAUSE_TIME);
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		can_draw_part1 = true;
	}

	std::cout << std::endl;
}

void Oxy::ReleaseFreeType(void)
{
	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}
int pen_x = 0;
int pen_y = 0;
unsigned char* pixels;

void ParseFont(int index, unsigned char* buffer)
{
	int local_tex_width;
	if (index != 72)
	{
		FT_Error err = FT_Set_Pixel_Sizes(Oxy::face, 0, index);
		if (err)
		{
			std::cout << "FAILED" << std::endl;
		}
	}

	local_tex_width = Oxy::tex_width;
	int arr_iter = 0;
	for (int i = 0; i < 128; ++i)
	{
		FT_Error err = FT_Load_Char(Oxy::face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &Oxy::face->glyph->bitmap;

		if (err)
		{
			std::cout << "ERROR" << std::endl;
			std::system("pause");
		}
		else
		{
			if (pen_x + bmp->width >= local_tex_width)
			{
				pen_x = 0;
				pen_y += ((Oxy::face->size->metrics.height >> 6) + 1);
				//	pen_y += current_state;
			}

			for (int row = 0; row < bmp->rows; ++row)
			{
				for (int col = 0; col < bmp->width; ++col)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					buffer[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			Oxy::info[arr_iter].x0 = pen_x;
			Oxy::info[arr_iter].y0 = pen_y;
			Oxy::info[arr_iter].x1 = pen_x + bmp->width;
			Oxy::info[arr_iter].y1 = pen_y + bmp->rows;

			Oxy::info[arr_iter].x_off = Oxy::face->glyph->bitmap_left;
			Oxy::info[arr_iter].y_off = Oxy::face->glyph->bitmap_top;
			Oxy::info[arr_iter].advance = Oxy::face->glyph->advance.x >> 6;
			arr_iter++;
			pen_x += bmp->width + 1;
		}
	}

	for (int i = 1040; i < 1104; ++i)
	{
		FT_Error err = FT_Load_Char(Oxy::face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &Oxy::face->glyph->bitmap;

		if (err)
		{
			std::cout << "ERROR" << std::endl;
			std::system("pause");
		}
		else
		{
			if (pen_x + bmp->width >= local_tex_width)
			{
				pen_x = 0;
				pen_y += ((Oxy::face->size->metrics.height >> 6) + 1);
				//	pen_y += current_state;
			}

			for (int row = 0; row < bmp->rows; ++row)
			{
				for (int col = 0; col < bmp->width; ++col)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			Oxy::info[arr_iter].x0 = pen_x;
			Oxy::info[arr_iter].y0 = pen_y;
			Oxy::info[arr_iter].x1 = pen_x + bmp->width;
			Oxy::info[arr_iter].y1 = pen_y + bmp->rows;

			Oxy::info[arr_iter].x_off = Oxy::face->glyph->bitmap_left;
			Oxy::info[arr_iter].y_off = Oxy::face->glyph->bitmap_top;
			Oxy::info[arr_iter].advance = Oxy::face->glyph->advance.x >> 6;
			arr_iter++;
			pen_x += bmp->width + 1;
		}
	}
}

void Oxy::InitFont(void)
{
	std::cout << "Set Size of your font: ";
	SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);

	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);

	Sleep(PAUSE_TIME);
	SetConsoleTextAttribute(hConsole, OK_COLOR);
	std::cout << "Successful!" << std::endl;
	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	font_height = 72;
	FT_Error err = FT_Set_Pixel_Sizes(face, 0, 72);
	std::cout << "Setting chosen size: ";
	// TODO: continue here
	int max_dm = ((1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(TOTAL_ANSCII)));
	int current_state = (1 + (face->size->metrics.height >> 6));
	while (Oxy::tex_width < max_dm)
		Oxy::tex_width <<= 1;
	Oxy::tex_height = Oxy::tex_width;
	unsigned char* dds_data = (unsigned char*)calloc((tex_width * tex_height * 4), 1);
	pixels = (unsigned char*)calloc(tex_width * tex_height, 1);
	ParseFont(72, pixels);
	ParseFont(48, pixels);
	ParseFont(36, pixels);
	for (int i = 28; i > 11; i -= 2)
	{
		ParseFont(i, pixels);
	}

	ParseFont(11, pixels);
	ParseFont(10, pixels);
	ParseFont(9, pixels);
	ParseFont(8, pixels);

		/*
		
		
		for (int i = 72; i > 0; i -= 2)
		{
			int local_tex_width;
			if (i != 72)
			{
				FT_Error err = FT_Set_Pixel_Sizes(face, 0, i);
				if (err)
				{
					std::cout << "FAILED" << std::endl;
				}
				local_tex_width = 1;
				int max_dm = ((1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(TOTAL_ANSCII)));

		//		while (local_tex_width < max_dm)
		//			local_tex_width <<= 1;
			}
			else
			{

			}
			local_tex_width = tex_width;
			int arr_iter = 0;
			for (int i = 0; i < 128; ++i)
			{
				FT_Error err = FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
				FT_Bitmap* bmp = &face->glyph->bitmap;

				if (err)
				{
					std::cout << "ERROR" << std::endl;
					std::system("pause");
				}
				else
				{
					if (pen_x + bmp->width >= local_tex_width)
					{
						pen_x = 0;
						pen_y += ((face->size->metrics.height >> 6) + 1);
					//	pen_y += current_state;
					}

					for (int row = 0; row < bmp->rows; ++row)
					{
						for (int col = 0; col < bmp->width; ++col)
						{
							int x = pen_x + col;
							int y = pen_y + row;
							pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
						}
					}

					info[arr_iter].x0 = pen_x;
					info[arr_iter].y0 = pen_y;
					info[arr_iter].x1 = pen_x + bmp->width;
					info[arr_iter].y1 = pen_y + bmp->rows;

					info[arr_iter].x_off = face->glyph->bitmap_left;
					info[arr_iter].y_off = face->glyph->bitmap_top;
					info[arr_iter].advance = face->glyph->advance.x >> 6;
					arr_iter++;
					pen_x += bmp->width + 1;
				}
			}


			for (int i = 1040; i < 1104; ++i)
			{
				FT_Error err = FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
				FT_Bitmap* bmp = &face->glyph->bitmap;

				if (err)
				{
					std::cout << "ERROR" << std::endl;
					std::system("pause");
				}
				else
				{
					if (pen_x + bmp->width >= local_tex_width)
					{
						pen_x = 0;
						pen_y += ((face->size->metrics.height >> 6) + 1);
					//	pen_y += current_state;
					}

					for (int row = 0; row < bmp->rows; ++row)
					{
						for (int col = 0; col < bmp->width; ++col)
						{
							int x = pen_x + col;
							int y = pen_y + row;
							pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
						}
					}

					info[arr_iter].x0 = pen_x;
					info[arr_iter].y0 = pen_y;
					info[arr_iter].x1 = pen_x + bmp->width;
					info[arr_iter].y1 = pen_y + bmp->rows;

					info[arr_iter].x_off = face->glyph->bitmap_left;
					info[arr_iter].y_off = face->glyph->bitmap_top;
					info[arr_iter].advance = face->glyph->advance.x >> 6;
					arr_iter++;
					pen_x += bmp->width + 1;
				}
			}



 
		}
		*/

		for (int i = 0; i < (tex_width * tex_height); ++i)
		{
			dds_data[i * 4] = 0;
			dds_data[i * 4 + 1] = 0;
			dds_data[i * 4 + 2] = 0;
			dds_data[i * 4 + 3] = pixels[i];
		}

		output += path_to_new_folder;
		output += "\\output_";
		output += std::to_string(size);
		output += "_" + std::to_string(tex_width);
		output += ".tga";
		stbi_write_tga(output.c_str(), tex_width, tex_height, 4, dds_data);
		free(dds_data);
		free(pixels);



}

void Oxy::CreateFolder(void)
{
	char buf[MAX_PATH];
	GetCurrentDirectoryA(sizeof(char) * MAX_PATH, buf);
	path_to_new_folder = buf;
	path_to_new_folder += "\\";
	std::string foldername = "Oxy.FontGenerated";
	path_to_new_folder += foldername;
	
	std::cout << "Creating a folder: ";
	if (std::experimental::filesystem::create_directory(path_to_new_folder))
	{
		Sleep(PAUSE_TIME);
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		std::cout << "Output folder is: ";
		SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
		std::cout << foldername << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		can_draw_part3 = true;
		suc_dir = true;

	}
	else
	{
		// @ But we must be sure that if the folder exists
		if (std::experimental::filesystem::exists(path_to_new_folder))
		{
			Sleep(PAUSE_TIME);
			SetConsoleTextAttribute(hConsole, OK_COLOR);
			std::cout << "Successful!" << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			std::cout << "Output (existed) folder is: ";
			SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
			std::cout << foldername << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			can_draw_part3 = true;
			suc_dir = true;
		}
		else
		{
			Sleep(PAUSE_TIME);
			SetConsoleTextAttribute(hConsole, ERROR_COLOR);
			std::cout << "Abort! Can't create a folder, maybe your OS is wrong or something happend." << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		}
	}
}

void Oxy::ManageCreationFile(void)
{
	std::string a;
	std::cout << "Write path to file (e.g. C:\\etc\\yourfile.ttf): ";
	SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
 
 
	std::cin >> a;
	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	std::string mask = a.substr(a.rfind(".") + 1);
	std::cout << "Loading Font file: ";
	// @ IDK, but maybe it supports otf format
	if (mask == "ttf" || mask == "ttc")
	{
		// @ Consider it's valid file for a while. . . 
		Sleep(PAUSE_TIME);
		FT_Error err = FT_New_Face(lib, a.c_str(), 0, &face);
		if (err)
		{
			SetConsoleTextAttribute(hConsole, ERROR_COLOR);
			std::cout << "Failed! Your file is invalid." << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		}
		else
		{
			SetConsoleTextAttribute(hConsole, OK_COLOR);
			std::cout << "Successful!" << std::endl;
			path_to_loaded_file = a;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			suc_file = true;
			can_draw_part2 = true;
		}

	}
	else
	{
		// @ Invalid path
		Sleep(PAUSE_TIME);
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "Failed! Your path is invalid." << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}

}


void Oxy::ReDrawInfo(void)
{
	CLEAR_WINDOW
	if (can_draw_part1)
	{
		std::cout << "Initializing Directx: ";
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);

		std::cout << "Initializing FreeType: ";
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}

	if (can_draw_part2)
	{
		std::cout << "Write path to file (e.g. C:\\etc\\yourfile.ttf): ";
		SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
		std::cout << path_to_loaded_file << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);

		std::cout << "Loading Font file: ";
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}

	// @ Folder
	if (can_draw_part3)
	{
		std::cout << "Creating a folder: ";

		if (std::experimental::filesystem::exists(path_to_new_folder))
		{
			SetConsoleTextAttribute(hConsole, OK_COLOR);
			std::cout << "Successful!" << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			std::cout << "Output (existed) folder is: ";
			SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
			std::cout << path_to_new_folder.substr(path_to_new_folder.rfind("\\")).c_str() << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		}
		else
		{
			SetConsoleTextAttribute(hConsole, OK_COLOR);
			std::cout << "Successful!" << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			std::cout << "Output folder is: ";
			SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
			std::cout << path_to_new_folder.substr(path_to_new_folder.rfind("\\")).c_str() << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		}
	
	}

	// @ Supports
	if (can_draw_part4)
	{

	}

	// @ Final 
	if (can_draw_part5)
	{
		std::cout << "Set Size of your font: ";
		SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
		std::cout << size << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}
}

void Oxy::CheckLanguages(void)
{
	FT_UInt gindex;
	FT_Error err;
	FT_ULong charcode = FT_Get_First_Char(face, &gindex);
	std::wstring bb = L"";
	wchar_t x = charcode;
	bb += x;
	while (gindex != 0)
	{
		charcode = FT_Get_Next_Char(face, charcode, &gindex);
		x = charcode;
		bb += x;
	}
	/*
	
	
	if (bb.find(' ',0))
	{

	}
	else
	{
		std::cout << "Your font file doesn't contain the symbol ' ' (space). Abort" << std::endl;
		std::system("pause");
		std::exit(1);
	}
	*/

	// @ English
	for (int i = 33; i < 123; ++i)
	{
		if (bb.find(i))
		{

		}
		else
		{
			std::cout << "UnSupported! English" << std::endl;
			std::system("pause");
			break;
		}
	}


	// @ Russian 
	for (int i = 1040; i < 1104; ++i)
	{
		if (bb.find(i))
		{

		}
		else
		{
			std::cout << "UnSupported! Russian" << std::endl;
			std::system("pause");
			break;
		}
	}

	// @ Japan, hiragana
	for (int i = 12352; i < 12448; ++i)
	{
		if (bb.find(i))
		{

		}
		else
		{
			std::cout << "UnSupported! Hiragana" << std::endl;
			std::system("pause");
			break;
		}
	}


	// @ Japan, Katakana
	for (int i = 12448; i < 12544; ++i)
	{
		if (bb.find(i))
		{

		}
		else
		{
			std::cout << "UnSupported! Katakana" << std::endl;
			std::system("pause");
			break;
		}
	}

	for (int i = 19000; i < 44001; ++i)
	{
		if (bb.find(i))
		{

		}
		else
		{
			std::cout << "UnSupported! Full Chineese and Japan" << std::endl;
			std::system("pause");
			break;
		}
	}
}

void Oxy::CreateTextureResolution(void)
{

}

void Oxy::CreateConfigs(void)
{
	std::ofstream file;
	std::string zz = output.substr(output.rfind("\\"));
	zz = zz.erase(zz.find("."));
	zz += ".ini";
	file.open(path_to_new_folder + zz);

	if (file.is_open())
	{
		file << "[symbol_coords]" << std::endl;
		file << "height = " << std::to_string(font_height) << std::endl;
		std::string fac = "00";
		std::string x = " = ";
		for (int i = 0; i < TOTAL_ANSCII; ++i)
		{
			if (i < 32)
			{
				x += "0";
				x += ", ";
				x += "0";
				x += ", ";
				x += "0";
				x += ", ";
				x += "0";
			}
			else
			{
				x += std::to_string(info[i].x0);
				x += ", ";
				x += std::to_string(info[i].y0);
				x += ", ";
				x += std::to_string(info[i].x1);
				x += ", ";
				x += std::to_string(info[i].y1);
			}


			if (i < 10)
			{
				fac = "00";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
				
			}


			if (i < 100 && i > 10)
			{
				fac = "0";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}
			
			if (i > 100)
			{
				fac = "";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}
		


			x = " = ";
		}
	}

	file.close();
}

int main()
{
	setlocale(LC_ALL, ".1251");
	// @ For DirectXTK
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		std::cout << "ComBaseApi initialization failed." << std::endl;
		return 1;
	}
 
	Oxy::HeaderMessage();
	Oxy::InitFreeType();
	
	Oxy::ManageCreationFile();
	Oxy::CheckLanguages();
	if (suc_file == true)	
	{
		Oxy::CreateFolder();
		if (suc_dir)
		{
			Oxy::InitFont();
			Oxy::CreateConfigs();
		}

	}
	Oxy::ReleaseFreeType();

	std::system("pause");

	return 0;
}
