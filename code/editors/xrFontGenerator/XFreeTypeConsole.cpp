// Author: phantom1020
#include <fstream>
#include "../../engine.vc2008/xrCore/xrCore.h"
#include "Kernel.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" 
 
// @ Думал что это будет удобно
#define PAUSE_AND_CLEAR system("pause"); system("cls");
#define CLEAR_WINDOW system("cls");

// @ Цвета для консоли
constexpr int DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
constexpr int ERROR_COLOR = 28;
constexpr int OK_COLOR = 26;
constexpr int COLOR_DEFAULT = 27;
constexpr int PAUSE_TIME = 1000;

XRay::PathList PathSystem;
XRay::ConvInfo ConverterInfo;
// @ If method ManageCreationFile is successful processing
bool suc_file = false;
bool suc_dir = false;

void XRay::InitFreeType()
{
	FT_Error err = FT_Init_FreeType(&ConverterInfo.lib);
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

void XRay::ReleaseFreeType()
{
	FT_Done_Face(ConverterInfo.face);
	FT_Done_FreeType(ConverterInfo.lib);
}

int g_count = 0;
int max_height_font = 0; // @ для вывода файлов, по сути обманка,
			// ибо в действительности мы уменьшаем размер шрифта и выводится
//должно под каждый размер, но мы делаем как ПЫС, то есть _size_800, _size_1600, _size, где size одно и то же число
void ParseFont(int index, int max_value)
{
	g_count++;

	ConverterInfo.FontHeig = index;
	FT_Error err = FT_Set_Pixel_Sizes(ConverterInfo.face, 0, index);
	if (err)
	{
		std::cout << "FAILED" << std::endl;
	}

	FT_Load_Char(ConverterInfo.face, 87, FT_LOAD_RENDER);
	XRay::info_copy.y_off = ConverterInfo.face->glyph->bitmap_top;

	float max_dm = ((1 + (ConverterInfo.face->size->metrics.height >> 6)) * ceilf(sqrtf(TOTAL_ANSCII)));
	int current_state = (1 + (ConverterInfo.face->size->metrics.height >> 6));

	// @ Просчитываем размер текстуры умножая на 2
	while (ConverterInfo.TexWid < max_dm)
		ConverterInfo.TexWid <<= 1;

	ConverterInfo.TexHeig = ConverterInfo.TexWid;

	if (!ConverterInfo.Pixels)
	{
		ConverterInfo.Pixels = (u8*)calloc(ConverterInfo.TexWid * ConverterInfo.TexHeig, 1);
	}

	if (index <= 0)
	{
		SetConsoleTextAttribute(XRay::hConsole, ERROR_COLOR);
		std::cout << "Can't create file, because your size is less or equal 0" << std::endl;
		SetConsoleTextAttribute(XRay::hConsole, DEFAULT_COLOR);
		return;
	}

	u32 pen_x = 0;
	u32 pen_y = 0;
	
	u32 local_tex_width = ConverterInfo.TexWid;
	int arr_iter = 0;
	int index_to_W = 0;
	int current_size = 0;

	// @ Проверяем на пробел
	err = FT_Load_Char(ConverterInfo.face, 0, FT_LOAD_RENDER); // Бёрем пустой символ (смотреть unicode таблицу)  (пробел почему-то не определят нормально, но он тоже выходит как пробел)
	if (!err)
	{
		if (!ConverterInfo.face->glyph->bitmap.rows && !ConverterInfo.face->glyph->bitmap.width)
		{
			// @ Пытаемся явно взять пробел
			err = FT_Load_Char(ConverterInfo.face, 32, FT_LOAD_RENDER);
			if (!err)
			{
				if (!ConverterInfo.face->glyph->bitmap.rows && !ConverterInfo.face->glyph->bitmap.width)
				{
					std::cout << "Your font doesn't have a 'space' symbol" << std::endl;
					system("pause");
					std::exit(1);
				}
			}

		}
	}

	for (u32 i = 32; i < 128; ++i)
	{
		err = FT_Load_Char(ConverterInfo.face, i, FT_LOAD_RENDER);
		FT_Bitmap* bmp = &ConverterInfo.face->glyph->bitmap;

		if (err)
		{
			std::cout << "ERROR" << std::endl;
			system("pause");
		}
		else
		{
			if (pen_x + bmp->width >= local_tex_width)
			{
				pen_x = 0;
				pen_y += ((ConverterInfo.face->size->metrics.height >> 6)) + 1;
			}

			if (i == 88)
			{
				index_to_W = arr_iter;
			}

			// @ +-.,
			if (i >= 42 && i <= 46)
			{
				current_size = int(XRay::info_copy.y_off - ConverterInfo.face->glyph->bitmap_top);
				pen_y += (u32)current_size;
			}

			// @ двоеточие и точка с запятой
			if (i == 58 || i == 59)
			{
				current_size = int(XRay::info_copy.y_off - ConverterInfo.face->glyph->bitmap_top);
				pen_y += (u32)current_size;
			}

			if (i >= 97 && i < 123)
			{
				current_size = int(XRay::info[index_to_W].y_off - ConverterInfo.face->glyph->bitmap_top);
				pen_y += (u32)current_size;
			}

			if (i == 32)
			{
				for (u32 row = 0; row < bmp->rows; ++row)
					for (u32 col = 0; col < bmp->width; ++col)
					{
						u32 x = pen_x + col;
						u32 y = pen_y + row;

						ConverterInfo.Pixels[y * local_tex_width + x] = 0;
					}
			}
			else for (u32 row = 0; row < bmp->rows; ++row)
			{
				for (u32 col = 0; col < bmp->width; ++col)
				{
					u32 x = pen_x + col;
					u32 y = pen_y + row;
					ConverterInfo.Pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			if (i >= 97 && i < 123)
			{
				if ((XRay::info[index_to_W].y_off - ConverterInfo.face->glyph->bitmap_top) > 0)
					pen_y -= current_size;
				else
					pen_y += abs(current_size);
			}

			if (i >= 42 && i <= 46)
			{
				if ((XRay::info_copy.y_off - ConverterInfo.face->glyph->bitmap_top) > 0)
					pen_y -= current_size;
				else
					pen_y += abs(current_size);
			}
			
			if (i == 58 || i == 59)
			{
				if ((XRay::info_copy.y_off - ConverterInfo.face->glyph->bitmap_top) > 0)
					pen_y -= current_size;
				else
					pen_y += abs(current_size);
			}

			XRay::info[arr_iter].x[0] = pen_x;
			XRay::info[arr_iter].y[0] = pen_y;
			XRay::info[arr_iter].x[1] = pen_x + bmp->width;
			XRay::info[arr_iter].y[1] = pen_y + bmp->rows;

			XRay::info[arr_iter].x_off = ConverterInfo.face->glyph->bitmap_left;
			XRay::info[arr_iter].y_off = ConverterInfo.face->glyph->bitmap_top;
			XRay::info[arr_iter].advance = ConverterInfo.face->glyph->advance.x >> 6;
			
			pen_x += bmp->width + 1;
			arr_iter++;
		}
	}

	int index_to_cyrillic_A = arr_iter;
	for (u32 i = 1040; i < 1104; ++i)
	{
		FT_Error err = FT_Load_Char(ConverterInfo.face, i, FT_LOAD_RENDER);
		FT_Bitmap* bmp = &ConverterInfo.face->glyph->bitmap;

		if (err)
		{
			std::cout << "ERROR" << std::endl;
			system("pause");
		}
		else
		{

			if (pen_x + bmp->width >= local_tex_width)
			{
				pen_x = 0;
				pen_y += ((ConverterInfo.face->size->metrics.height >> 6) + 1);
			}

			if (i >= 1072 || i == 1049)
			{
				current_size = int(XRay::info[index_to_cyrillic_A].y_off - ConverterInfo.face->glyph->bitmap_top);
				pen_y += (u32)current_size;
			}

			for (u32 row = 0; row < bmp->rows; ++row)
			{
				for (u32 col = 0; col < bmp->width; ++col)
				{
					u32 x = pen_x + col;
					u32 y = pen_y + row;
					ConverterInfo.Pixels[y * local_tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}
			
			if (i >= 1072 || i == 1049)
			{
				if ((XRay::info[index_to_cyrillic_A].y_off - ConverterInfo.face->glyph->bitmap_top) > 0)
				{
					pen_y -= current_size;
				}

				if ((XRay::info[index_to_cyrillic_A].y_off - ConverterInfo.face->glyph->bitmap_top) < 0)
				{
					pen_y += abs(current_size);
				}
			}

			for (u32 CordIter = 0; CordIter < 2; CordIter++)
			{
				XRay::info[arr_iter].x[CordIter] = pen_x + ((CordIter == 1) ? bmp->width : 0);
				XRay::info[arr_iter].y[CordIter] = pen_y + ((CordIter == 1) ? bmp->rows  : 0);
			}

			XRay::info[arr_iter].x_off = ConverterInfo.face->glyph->bitmap_left;
			XRay::info[arr_iter].y_off = ConverterInfo.face->glyph->bitmap_top;
			XRay::info[arr_iter].advance = ConverterInfo.face->glyph->advance.x >> 6;
			arr_iter++;
			pen_x += bmp->width + 1;
		}
	}

	u8* dds_data = (u8*)calloc(ConverterInfo.TexWid * ConverterInfo.TexHeig * 4, 1);

	for (u32 i = 0; i < (ConverterInfo.TexWid * ConverterInfo.TexHeig); ++i)
	{
		dds_data[i * 4] = 0;
		dds_data[i * 4 + 1] = 0;
		dds_data[i * 4 + 2] = 0;
		dds_data[i * 4 + 3] = ConverterInfo.Pixels[i];
	}

	if (g_count)
	{
		PathSystem.FileOutName += PathSystem.PathOutName;
		PathSystem.FileOutName += PathSystem.FileName + "_";

		switch (g_count)
		{
			case 1:
			{
				PathSystem.FileOutName += std::to_string(ConverterInfo.FontHeig);
				PathSystem.FileOutName += "_" + std::to_string(1600);
				PathSystem.FileOutName += ".tga";
				max_height_font = ConverterInfo.FontHeig;
			} break;

			case 2:
			{
				PathSystem.FileOutName += std::to_string(max_height_font);
				PathSystem.FileOutName += "_" + std::to_string(1024);
				PathSystem.FileOutName += ".tga";
			} break;

			case 3:
			{
				PathSystem.FileOutName += std::to_string(max_height_font);
				PathSystem.FileOutName += "_" + std::to_string(800);
				PathSystem.FileOutName += ".tga";
				g_count = 0;
			} break;
		}
	}

	stbi_write_tga(PathSystem.FileOutName.c_str(), ConverterInfo.TexWid, ConverterInfo.TexHeig, 4, dds_data);
	free(dds_data);
	free(ConverterInfo.Pixels);
	ZeroMemory(&ConverterInfo.Pixels, sizeof(ConverterInfo.Pixels)); 

	// @ Создание ini файла
	std::ofstream INIFile;
	xr_string output_copy = PathSystem.FileOutName;
	INIFile.open((PathSystem.FileOutName.erase(PathSystem.FileOutName.rfind(".")) + ".ini").c_str());

	if (INIFile.is_open())
	{
		auto MakeNulls = [](std::string &x)
		{
			for (u32 iter = 0; iter < 3; iter++)
			{
				x += "0";
				x += ", ";
			} x += "0";
		};

		INIFile << "[symbol_coords]" << std::endl;
		INIFile << "height = " << std::to_string(ConverterInfo.FontHeig) << std::endl;
		std::string fac = "00";
		std::string x = " = ";

		for (u32 i = 0; i < 32; ++i)
		{
			MakeNulls(x);

			fac = (i < 10) ? "00" : "0";
			fac += std::to_string(i);
			INIFile << fac;
			INIFile << x;
			INIFile << std::endl;

			x = " = ";
		}

		// @ Идёт Латиница и служебные символы (снача служебные символы, затем латынь)
		size_t local_it = 0; // @ Нужно учитывать что перессылка разная, особенно с данным массивом
		for (u32 i = 32; i < 128; ++i)
		{
			for (u32 CordIter = 0; CordIter < 2; CordIter++)
			{
				x += std::to_string(XRay::info[local_it].x[CordIter]) + ", ";
				x += std::to_string(XRay::info[local_it].y[CordIter]) + ((CordIter == 0) ? ", " : "");
			}
			
			fac = (i < 100 && i > 10) ? "0" : "";
			fac += std::to_string(i);
			INIFile << fac;
			INIFile << x;
			INIFile << std::endl;

			x = " = ";
			local_it++;
		}

		auto WriteToINI = [&INIFile, &fac, &x](size_t &Iterator)
		{
			fac = std::to_string(Iterator);
			INIFile << fac;
			INIFile << x;
			INIFile << std::endl;

			x = " = ";
		};

		// @ ПЫСовские мусорные символы, в текстурах они не содержаться (не то что в оригинале!)
		for (size_t i = 128; i < 192; ++i)
		{
			MakeNulls(x);
			WriteToINI(i);
		}

		// @ Идёт кириллица
		for (size_t i = 192; i < 256; ++i)
		{
			for (u32 CordIter = 0; CordIter < 2; CordIter++)
			{
				x += std::to_string(XRay::info[local_it].x[CordIter]) + ", ";
				x += std::to_string(XRay::info[local_it].y[CordIter]) + ((CordIter == 0) ? ", " : "");
			}

			WriteToINI(i);
			local_it++;
		}
	}
	PathSystem.FileOutName = "";
	INIFile.close();

	if (ConverterInfo.bHaveTexconv)
	{
		// @ Записали уже .tga -> конвертируем в соответствующий размер под dds
		system(("texconv.exe -f DXT5 " + output_copy + " -o " + PathSystem.PathOutName.c_str() +" -y").c_str());
	}
	else
	{
		std::cout << "Sorry, but you don't have in your folder (where's your application)";
		SetConsoleTextAttribute(XRay::hConsole, ERROR_COLOR);
		std::cout << "texconv.exe";
		SetConsoleTextAttribute(XRay::hConsole, DEFAULT_COLOR);
		std::cout << "!" << std::endl;
	}
}

void XRay::CheckTexConv()
{
	ConverterInfo.bHaveTexconv = std::experimental::filesystem::exists("texconv.exe");
}

void XRay::CreateGSCFonts()
{
	u32 your_size;
	std::cin >> your_size;

	if (your_size)
	{
		for(u32 Iter = 0; Iter < 3; Iter++)
			ParseFont(your_size - Iter * 2, your_size); // 0 - 2 - 4
	}
}

void XRay::InitFont()
{
	std::cout << "Set Size of your font: ";
	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	ConverterInfo.Pixels = nullptr;
	CreateGSCFonts();

	if (!g_count)
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

void XRay::CreateFolder(void)
{	
	auto GoodPrint = []()
	{
		SetConsoleTextAttribute(hConsole, OK_COLOR);
		std::cout << "Successful!" << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		std::cout << "Output folder is: ";
		SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
		std::cout << PathSystem.PathOutName << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		suc_dir = true;
	};

	std::cout << "Creating a folder: ";
	if (std::experimental::filesystem::create_directory(PathSystem.PathOutName))
	{
		GoodPrint();
	}
	else
	{
		// @ But we must be sure that if the folder exists
		if (std::experimental::filesystem::exists(PathSystem.PathOutName))
		{
			GoodPrint();
		}
		else
		{
			SetConsoleTextAttribute(hConsole, ERROR_COLOR);
			std::cout << "Abort! Can't create a folder, maybe your OS is wrong or something happend." << std::endl;
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
		}
	}
}

void XRay::ManageCreationFile()
{
	xr_string a;
	std::cout << "Write path to file (e.g. C:\\etc\\yourfile.ttf): ";
	SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
 
	std::cin >> a;
	SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	xr_string mask = a.substr(a.rfind(".") + 1);

	// Init PathList
	PathSystem.FileName = a.erase(a.rfind("."));

	xr_vector<xr_string> NewStrSpl = PathSystem.FileName.Split('\\');
	NewStrSpl.erase(NewStrSpl.end() - 1);

	PathSystem.PathName = "";
	for (xr_string &PicePath : NewStrSpl)
	{
		PathSystem.PathName += PicePath + '\\';
	}
	PathSystem.FileName = PathSystem.FileName.substr(3, PathSystem.FileName.length() - 2);
	PathSystem.PathOutName = PathSystem.PathName + "XRayFonts\\";

	// Font converting - start
	a += ".";
	a += mask;
	std::cout << "Loading Font file: ";

	if (mask == "ttf" || mask == "ttc")
	{
		// @ Consider it's valid file for a while. . . 
		FT_Error err = FT_New_Face(ConverterInfo.lib, a.c_str(), 0, &ConverterInfo.face);
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
			SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
			suc_file = true;
		}
	}
	else
	{
		// @ Invalid path
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "Failed! Your path is invalid." << std::endl;
		SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
	}
}

int main()
{
	setlocale(LC_ALL, ".1251");
	XRay::HeaderMessage();

	auto ConverterCallback = []()
	{
		XRay::InitFreeType();

		XRay::ManageCreationFile();
		if (suc_file == true)
		{
			XRay::CreateFolder();
			XRay::CheckTexConv();

			if (suc_dir)
				XRay::InitFont();
		}
		XRay::ReleaseFreeType();
		system("pause");
	};

	do
	{
		ConverterCallback();
	} while (true);

	return 0;
}