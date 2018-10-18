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
	pIni = new CInifile(marshal.marshal_as<const char*>(Filesystem::GetPathToResource(CONFIG, fileName)), readOnly, load, saveAtEnd);
}

Ini::~Ini()
{
	delete pIni;
}

bool Ini::IsSectionExist(String^ section)
{
	return pIni->section_exist(marshal.marshal_as<const char*>(section));
}

bool Ini::IsLineExist(String^ section, String^ line)
{
	return pIni->line_exist(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

UInt32 Ini::GetLineCount(String^ section)
{
	return pIni->line_count(marshal.marshal_as<const char*>(section));
}

bool Ini::ReadBool(String^ section, String^ line)
{
	return pIni->r_bool(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

UInt64 Ini::ReadClassId(String^ section)
{
	return ReadClassId(section, "class");
}

UInt64 Ini::ReadClassId(String^ section, String^ line)
{
	return pIni->r_clsid(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

SByte Ini::ReadByte(String^ section, String^ line)
{
	return pIni->r_s8(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

Byte Ini::ReadUByte(String^ section, String^ line)
{
	return pIni->r_u8(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

Int16 Ini::ReadShort(String^ section, String^ line)
{
	return pIni->r_s16(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

UInt16 Ini::ReadUShort(String^ section, String^ line)
{
	return pIni->r_u16(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

Int32 Ini::ReadInt(String^ section, String^ line)
{
	return pIni->r_s32(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

UInt32 Ini::ReadUInt(String^ section, String^ line)
{
	return pIni->r_u32(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

UInt64 Ini::ReadULong(String^ section, String^ line)
{
	return pIni->r_u64(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

float Ini::ReadFloat(String^ section, String^ line)
{
	return pIni->r_float(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

Int32 Ini::ReadColor(String^ section, String^ line)
{
	return pIni->r_color(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
}

String^ Ini::ReadString(String^ section, String^ line)
{
	return gcnew String(pIni->r_string(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line)));
}

Vector2^ Ini::ReadVector2(String^ section, String^ line)
{
	Fvector2 vector = pIni->r_fvector2(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
	return gcnew Vector2(vector.x, vector.y);
}

Vector3^ Ini::ReadVector3(String^ section, String^ line)
{
	Fvector3 vector = pIni->r_fvector3(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
	return gcnew Vector3(vector.x, vector.y, vector.z);
}

Vector4^ Ini::ReadVector4(String^ section, String^ line)
{
	Fvector4 vector = pIni->r_fvector4(marshal.marshal_as<const char*>(section), marshal.marshal_as<const char*>(line));
	return gcnew Vector4(vector.x, vector.y, vector.z, vector.w);
}

void Ini::SaveAs(String^ fileName)
{
	pIni->save_as(marshal.marshal_as<const char*>(fileName));
}

String^ Ini::ToString()
{
	return "Ini: " + gcnew String(pIni->get_as_string().c_str());
}