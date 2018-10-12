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
	pIni = new CInifile(Marshal.marshal_as<const char*>(Filesystem::GetPathToResource(CONFIG, fileName)), readOnly, load, saveAtEnd);
}

Ini::~Ini()
{
	delete pIni;
}

bool Ini::IsSectionExist(String^ section)
{
	return pIni->section_exist(Marshal.marshal_as<const char*>(section));
}

bool Ini::IsLineExist(String^ section, String^ line)
{
	return pIni->line_exist(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

u32 Ini::GetLineCount(String^ section)
{
	return pIni->line_count(Marshal.marshal_as<const char*>(section));
}

bool Ini::r_bool(String^ section, String^ line)
{
	return pIni->r_bool(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

u8 Ini::r_u8(String^ section, String^ line)
{
	return pIni->r_u8(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

u16 Ini::r_u16(String^ section, String^ line)
{
	return pIni->r_u16(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

u32 Ini::r_u32(String^ section, String^ line)
{
	return pIni->r_u32(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

u64 Ini::r_u64(String^ section, String^ line)
{
	return pIni->r_u64(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

s8 Ini::r_s8(String^ section, String^ line)
{
	return pIni->r_s8(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

s16 Ini::r_s16(String^ section, String^ line)
{
	return pIni->r_s16(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

s32 Ini::r_s32(String^ section, String^ line)
{
	return pIni->r_s32(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

float Ini::r_float(String^ section, String^ line)
{
	return pIni->r_float(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line));
}

String^ Ini::r_string(String^ section, String^ line)
{
	return gcnew String(pIni->r_string(Marshal.marshal_as<const char*>(section), Marshal.marshal_as<const char*>(line)));
}

void Ini::SaveAs(String^ fileName)
{
	pIni->save_as(Marshal.marshal_as<const char*>(fileName));
}

String^ Ini::ToString()
{
	return "Ini: " + gcnew String(pIni->get_as_string().c_str());
}