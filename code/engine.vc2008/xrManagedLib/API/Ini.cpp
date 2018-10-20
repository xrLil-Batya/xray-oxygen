#include "stdafx.h"
#include "Ini.h"
#include "Log.h"

using namespace XRay;

Ini::Ini(String^ fileName) : Ini(fileName, true)
{
}

Ini::Ini(String^ fileName, bool readOnly) : Ini(fileName, readOnly, true)
{
}

Ini::Ini(String^ fileName, bool readOnly, bool load) : Ini(fileName, readOnly, load, true)
{
}

Ini::Ini(String^ fileName, bool readOnly, bool load, bool saveAtEnd)
{
	string1024 PathToResource;
	ConvertDotNetStringToAscii(Filesystem::GetPathToResource(CONFIG, fileName), PathToResource);
	pNativeObject = new CInifile(PathToResource, readOnly, load, saveAtEnd);
}

Ini::~Ini()
{
	delete pNativeObject;
}

bool Ini::IsSectionExist(String^ section)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	return pNativeObject->section_exist(Section);
}

bool Ini::IsLineExist(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->line_exist(Section, Line);
}

UInt32 Ini::GetLineCount(String^ section)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	return pNativeObject->line_count(Section);
}

bool Ini::ReadBool(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_bool(Section, Line);
}

UInt64 Ini::ReadClassId(String^ section)
{
	return ReadClassId(section, "class");
}

UInt64 Ini::ReadClassId(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_clsid(Section, Line);
}

SByte Ini::ReadByte(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_s8(Section, Line);
}

Byte Ini::ReadUByte(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_u8(Section, Line);
}

Int16 Ini::ReadShort(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_s16(Section, Line);
}

UInt16 Ini::ReadUShort(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_u16(Section, Line);
}

Int32 Ini::ReadInt(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_s32(Section, Line);
}

UInt32 Ini::ReadUInt(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_u32(Section, Line);
}

UInt64 Ini::ReadULong(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_u64(Section, Line);
}

float Ini::ReadFloat(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_float(Section, Line);
}

Int32 Ini::ReadColor(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return pNativeObject->r_color(Section, Line);
}

String^ Ini::ReadString(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	return gcnew String(pNativeObject->r_string(Section, Line));
}

Vector2^ Ini::ReadVector2(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	Fvector2 vector = pNativeObject->r_fvector2(Section, Line);
	return gcnew Vector2(vector.x, vector.y);
}

Vector3^ Ini::ReadVector3(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	Fvector3 vector = pNativeObject->r_fvector3(Section, Line);
	return gcnew Vector3(vector.x, vector.y, vector.z);
}

Vector4^ Ini::ReadVector4(String^ section, String^ line)
{
	string512 Section;
	ConvertDotNetStringToAscii(section, Section);
	string512 Line;
	ConvertDotNetStringToAscii(line, Line);

	Fvector4 vector = pNativeObject->r_fvector4(Section, Line);
	return gcnew Vector4(vector.x, vector.y, vector.z, vector.w);
}

void Ini::SaveAs(String^ fileName)
{
	string512 FileNameStr;
	ConvertDotNetStringToAscii(fileName, FileNameStr);
	pNativeObject->save_as(FileNameStr);
}

String^ Ini::ToString()
{
	return "Ini: " + gcnew String(pNativeObject->get_as_string().c_str());
}