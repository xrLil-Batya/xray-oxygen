#include "stdafx.h"
#include "INIParser.h"

using namespace XRay;

bool INIParser::SaveAs(string file)
{
	return pSettings->save_as(Marshal.marshal_as<const char*>(file));
}

System::String^ INIParser::GetAsString()
{
	return gcnew System::String(pSettings->get_as_string().c_str());
}

bool INIParser::LineExist(string S, string L)
{
	return pSettings->line_exist(Marshal.marshal_as<const char*>(S), Marshal.marshal_as<const char*>(L));
}

u32 INIParser::LineCount(string sect)
{
	return pSettings->line_count(Marshal.marshal_as<const char*>(sect));
}

bool INIParser::SectionExist(string sect)
{
	return pSettings->section_exist(Marshal.marshal_as<const char*>(sect));
}

// Unsigned
u8 INIParser::r_u8(string Sect, string Val)
{
	return pSettings->r_u8(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

u16 INIParser::r_u16(string Sect, string Val)
{
	return pSettings->r_u16(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

u32 INIParser::r_u32(string Sect, string Val)
{
	return pSettings->r_u32(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

u64 INIParser::r_u64(string Sect, string Val)
{
	return pSettings->r_u64(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

// Signed
s8 INIParser::r_s8(string Sect, string Val)
{
	return pSettings->r_s8(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

s16 INIParser::r_s16(string Sect, string Val)
{
	return pSettings->r_s16(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

s32 INIParser::r_s32(string Sect, string Val)
{
	return pSettings->r_s32(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

System::String^ INIParser::r_string(string Sect, string Val)
{
	return gcnew System::String(pSettings->r_string(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val)));
}

bool INIParser::r_bool(string Sect, string Val)
{
	return pSettings->r_bool(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}

float INIParser::r_float(string Sect, string Val)
{
	return pSettings->r_float(Marshal.marshal_as<const char*>(Sect), Marshal.marshal_as<const char*>(Val));
}