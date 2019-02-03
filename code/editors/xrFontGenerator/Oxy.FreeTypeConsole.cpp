// Author: phantom1020

#include "Kernel.h"

// @ собственно для tga
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" 

#include <fstream>
 
// @ Думал что это будет удобно
#define PAUSE_AND_CLEAR std::system("pause"); std::system("cls");
#define CLEAR_WINDOW std::system("cls");

// @ Цвета для консоли
constexpr int DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
constexpr int ERROR_COLOR = 28;
constexpr int OK_COLOR = 26;
constexpr int COLOR_DEFAULT = 27;
constexpr int PAUSE_TIME = 1000;

// @ If method ManageCreationFile is successful processing
bool suc_file = false;
bool suc_dir = false;
// @ Да можно было бы это реализовать, но мы ограничиваемся только рузге и fucking hell man
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
	}

	std::cout << std::endl;
}

void Oxy::ReleaseFreeType(void)
{
	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

int g_count = 0;
int max_height_font = 0; // @ для вывода файлов, по сути обманка,
			// ибо в действительности мы уменьшаем размер шрифта и выводится
//должно под каждый размер, но мы делаем как ПЫС, то есть _size_800, _size_1600, _size, где size одно и то же число
void ParseFont(int index, int max_value)
{
	g_count++;


	Oxy::font_height = index;
	FT_Error err = FT_Set_Pixel_Sizes(Oxy::face, 0, index);
	if (err)
	{
		std::cout << "FAILED" << std::endl;
	}

	FT_Load_Char(Oxy::face, 87, FT_LOAD_RENDER);
	Oxy::info_copy.y_off = Oxy::face->glyph->bitmap_top;

	int max_dm = ((1 + (Oxy::face->size->metrics.height >> 6)) * ceilf(sqrtf(TOTAL_ANSCII)));
	int current_state = (1 + (Oxy::face->size->metrics.height >> 6));

	// @ Просчитываем размер текстуры умножая на 2
	while (Oxy::tex_width < max_dm)
		Oxy::tex_width <<= 1;

	Oxy::tex_height = Oxy::tex_width;

	if (Oxy::pixels == nullptr || !Oxy::pixels)
	{
		Oxy::pixels = (unsigned char*)calloc(Oxy::tex_width * Oxy::tex_height, 1);
	}

	if (index <= 0)
	{
		SetConsoleTextAttribute(Oxy::hConsole, ERROR_COLOR);
		std::cout << "Can't create file, because your size is less or equal 0" << std::endl;
		SetConsoleTextAttribute(Oxy::hConsole, DEFAULT_COLOR);
		return;
	}

	int local_tex_width;

	int pen_x = 0;
	int pen_y = 0;
	

	local_tex_width = Oxy::tex_width;
	int arr_iter = 0;
	int index_to_W = 0;
	int current_size = 0;

	for (int i = 32; i < 128; ++i)
	{
		if (i == 32)
		{ // @ Проверяем на пробел
			FT_Error err = FT_Load_Char(Oxy::face, 0, FT_LOAD_RENDER); // Бёрем пустой символ (смотреть unicode таблицу)  (пробел почему-то не определят нормально, но он тоже выходит как пробел)
			if (!err)
			{
				if (!Oxy::face->glyph->bitmap.rows && !Oxy::face->glyph->bitmap.width)
				{
					// @ Пытаемся явно взять пробел
					err = FT_Load_Char(Oxy::face, 32, FT_LOAD_RENDER);
					if (!err)
					{
						if (!Oxy::face->glyph->bitmap.rows && !Oxy::face->glyph->bitmap.width)
						{
							std::cout << "Your font doesn't have a 'space' symbol" << std::endl;
							std::system("pause");
							std::exit(1);
						}
					}

				}
			}

		}
		else
		{
			FT_Error err = FT_Load_Char(Oxy::face, i, FT_LOAD_RENDER);
		}

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
				pen_y += ((Oxy::face->size->metrics.height >> 6)) + 1;

			}

			if (i == 88)
			{
				index_to_W = arr_iter;
			}

			// @ +-.,
			if (i >= 42 && i <= 46)
			{
				current_size = (Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top);
				pen_y += current_size;
			}

			// @ двоеточие и точка с запятой
			if (i == 58 || i == 59)
			{
				current_size = (Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top);
				pen_y += current_size;
			}

			if (i >= 97 && i < 123)
			{

				current_size = (Oxy::info[index_to_W].y_off - Oxy::face->glyph->bitmap_top);
				pen_y += current_size;
			}

			if (i == 32)
			{
				for (int row = 0; row < bmp->rows; ++row)
				{
					for (int col = 0; col < bmp->width; ++col)
					{

						int x = pen_x + col;
						int y = pen_y + row;


						Oxy::pixels[y * local_tex_width + x] = 0;
					}
				}
			}
			else
			{
				for (int row = 0; row < bmp->rows; ++row)
				{
					for (int col = 0; col < bmp->width; ++col)
					{

						int x = pen_x + col;
						int y = pen_y + row;


						Oxy::pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
					}
				}
			}


			Oxy::info[arr_iter].x0 = pen_x;

			if (i >= 97 && i < 123)
			{
				if ((Oxy::info[index_to_W].y_off - Oxy::face->glyph->bitmap_top) > 0)
				{
					pen_y -= current_size;
				}
				
				if ((Oxy::info[index_to_W].y_off - Oxy::face->glyph->bitmap_top) < 0)
				{
					pen_y += abs(current_size);
				}
			}

			if (i >= 42 && i <= 46)
			{
				if ((Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top) > 0)
				{
					pen_y -= current_size;
				}

				if ((Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top) < 0)
				{
					pen_y += abs(current_size);
				}
			}
			
			if (i == 58 || i == 59)
			{
				if ((Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top) > 0)
				{
					pen_y -= current_size;
				}

				if ((Oxy::info_copy.y_off - Oxy::face->glyph->bitmap_top) < 0)
				{
					pen_y += abs(current_size);
				}
			}

			Oxy::info[arr_iter].y0 = pen_y;
			Oxy::info[arr_iter].x1 = pen_x + bmp->width;
			Oxy::info[arr_iter].y1 = pen_y + bmp->rows;

			Oxy::info[arr_iter].x_off = Oxy::face->glyph->bitmap_left;
			Oxy::info[arr_iter].y_off = Oxy::face->glyph->bitmap_top;
			Oxy::info[arr_iter].advance = Oxy::face->glyph->advance.x >> 6;
			
			pen_x += bmp->width + 1;
			arr_iter++;
 
		}
	}
	int index_to_cyrillic_A = arr_iter;
	for (int i = 1040; i < 1104; ++i)
	{
		FT_Error err = FT_Load_Char(Oxy::face, i, FT_LOAD_RENDER);
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
			}

			if (i >= 1072 || i == 1049)
			{
				current_size = (Oxy::info[index_to_cyrillic_A].y_off - Oxy::face->glyph->bitmap_top);
				pen_y += current_size;
			}

			for (int row = 0; row < bmp->rows; ++row)
			{
				for (int col = 0; col < bmp->width; ++col)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					Oxy::pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}
			
			if (i >= 1072 || i == 1049)
			{
				if ((Oxy::info[index_to_cyrillic_A].y_off - Oxy::face->glyph->bitmap_top) > 0)
				{
					pen_y -= current_size;
				}

				if ((Oxy::info[index_to_cyrillic_A].y_off - Oxy::face->glyph->bitmap_top) < 0)
				{
					pen_y += abs(current_size);
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

	unsigned char* dds_data = (unsigned char*)calloc(Oxy::tex_width * Oxy::tex_height * 4, 1);

	for (int i = 0; i < (Oxy::tex_width * Oxy::tex_height); ++i)
	{
		dds_data[i * 4] = 0;
		dds_data[i * 4 + 1] = 0;
		dds_data[i * 4 + 2] = 0;
		dds_data[i * 4 + 3] = Oxy::pixels[i];
	}

	if (g_count == 1)
	{
		Oxy::output += Oxy::path_to_new_folder;
		Oxy::output += "\\" + Oxy::filename + "_";
		Oxy::output += std::to_string(Oxy::font_height);
		Oxy::output += "_" + std::to_string(1600);
		Oxy::output += ".tga";
		max_height_font = Oxy::font_height;

	}

	if (g_count == 2)
	{
		Oxy::output += Oxy::path_to_new_folder;
		Oxy::output += "\\" + Oxy::filename + "_";
		Oxy::output += std::to_string(max_height_font);
		Oxy::output += "_" + std::to_string(1024);
		Oxy::output += ".tga";
	}

	if (g_count == 3)
	{
		Oxy::output += Oxy::path_to_new_folder;
		Oxy::output += "\\" + Oxy::filename + "_";
		Oxy::output += std::to_string(max_height_font);
		Oxy::output += "_" + std::to_string(800);
		Oxy::output += ".tga";
		g_count = 0;
	}

	stbi_write_tga(Oxy::output.c_str(), Oxy::tex_width, Oxy::tex_height, 4, dds_data);
	free(dds_data);
	free(Oxy::pixels);
	ZeroMemory(&Oxy::pixels, sizeof(Oxy::pixels)); 

	// @ Создание ini файла
	std::ofstream file;
	std::string output_copy = Oxy::output;
	file.open(Oxy::output.erase(Oxy::output.rfind(".")) + ".ini");

	if (file.is_open())
	{
		file << "[symbol_coords]" << std::endl;
		file << "height = " << std::to_string(Oxy::font_height) << std::endl;
		std::string fac = "00";
		std::string x = " = ";

		for (int i = 0; i < 32; ++i)
		{
			x += "0";
			x += ", ";
			x += "0";
			x += ", ";
			x += "0";
			x += ", ";
			x += "0";

			if (i < 10)
			{
				fac = "00";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}
			else
			{
				fac = "0";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}

			x = " = ";
		}
		// @ Идёт Латиница и служебные символы (снача служебные символы, затем латынь)
		int local_it = 0; // @ Нужно учитывать что перессылка разная, особенно с данным массивом
		for (int i = 32; i < 128; ++i)
		{
			x += std::to_string(Oxy::info[local_it].x0);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].y0);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].x1);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].y1);
			
			if (i < 100 && i > 10)
			{
				fac = "0";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}

			if (i >= 100)
			{
				fac = "";
				fac += std::to_string(i);
				file << fac;
				file << x;
				file << std::endl;
			}



			x = " = ";
			local_it++;
		}

		// @ ПЫСовские мусорные символы, в текстурах они не содержаться (не то что в оригинале!)
		for (int i = 128; i < 192; ++i)
		{
			x += "0";
			x += ", ";
			x += "0";
			x += ", ";
			x += "0";
			x += ", ";
			x += "0";

			fac = "";
			fac += std::to_string(i);
			file << fac;
			file << x;
			file << std::endl;

			x = " = ";
		}

		// @ Идёт кириллица
		for (int i = 192; i < 256; ++i)
		{
			x += std::to_string(Oxy::info[local_it].x0);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].y0);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].x1);
			x += ", ";
			x += std::to_string(Oxy::info[local_it].y1);

			fac = "";
			fac += std::to_string(i);
			file << fac;
			file << x;
			file << std::endl;

			x = " = ";
			local_it++;
		}
	}
	Oxy::output = "";
	file.close();

	if (Oxy::have_we_texconv)
	{
		// @ Записали уже .tga -> конвертируем в соответствующий размер под dds
		std::system(("texconv -f DXT5 " + output_copy + " -o Generated\\ -y").c_str());

		// @ Удаляем файл, то есть tga текстуру!
		std::remove(output_copy.c_str());
	}
	else
	{
		std::cout << "Sorry, but you don't have in your folder (where's your application)";
		SetConsoleTextAttribute(Oxy::hConsole, ERROR_COLOR);
		std::cout << "texconv.exe";
		SetConsoleTextAttribute(Oxy::hConsole, DEFAULT_COLOR);
		std::cout << "!" << std::endl;
	}
}

void Oxy::CheckTexConv(void)
{
	if (std::experimental::filesystem::exists("texconv.exe"))
	{
		Oxy::have_we_texconv = true;
	}
	else
	{
		Oxy::have_we_texconv = false;
	}
}

void Oxy::CreateGSCFonts()
{
	int your_size;
	std::cin >> your_size;

	if (your_size <= 0)
	{
		return;
	}
	ParseFont(your_size, your_size);
	ParseFont(your_size - 2, your_size);
	ParseFont(your_size - 4, your_size);
}


void Oxy::InitFont(void)
{
	std::cout << "Set Size of your font: ";
	SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);

	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	Oxy::pixels = nullptr;
	CreateGSCFonts();

	if (g_count == 0)
	{
		Sleep(PAUSE_TIME);
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "Failed!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}
}

void Oxy::CreateFolder(void)
{
	char buf[MAX_PATH];
	GetCurrentDirectoryA(sizeof(char) * MAX_PATH, buf);
	path_to_new_folder = buf;
	path_to_new_folder += "\\";
	std::string foldername = "Generated";
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
	filename = a.erase(a.rfind("."));
	a += ".";
	a += mask;
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

int main()
{
	setlocale(LC_ALL, ".1251");
 
	Oxy::HeaderMessage();
	Oxy::InitFreeType();
	
	Oxy::ManageCreationFile();
	if (suc_file == true)	
	{
		Oxy::CreateFolder();
		Oxy::CheckTexConv();
		if (suc_dir)
		{
			Oxy::InitFont();
		}

	}
	Oxy::ReleaseFreeType();
	std::system("pause");

	return 0;
}
