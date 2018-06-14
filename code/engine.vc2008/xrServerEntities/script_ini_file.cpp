////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"
#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"

CScriptIniFile::CScriptIniFile(IReader *F, const char* path) : inherited(F, path)
{
}

CScriptIniFile::CScriptIniFile(const char* szFileName, bool ReadOnly, BOOL bLoadAtStart, BOOL SaveAtEnd) : inherited(update(szFileName), ReadOnly ? TRUE : FALSE, bLoadAtStart, SaveAtEnd)
{
}

CScriptIniFile::~CScriptIniFile()
{
}

const char*	CScriptIniFile::update(const char* file_name)
{
	string_path	S1;
	FS.update_path(S1, "$game_config$", file_name);
	return *shared_str(S1);
}

bool CScriptIniFile::line_exist(const char* S, const char* L)
{
	return !!inherited::line_exist(S, L);
}

bool CScriptIniFile::section_exist(const char* S)
{
	return !!inherited::section_exist(S);
}

int CScriptIniFile::r_clsid(const char* S, const char* L)
{
	return object_factory().script_clsid(inherited::r_clsid(S, L));
}

bool CScriptIniFile::r_bool(const char* S, const char* L)
{
	return !!inherited::r_bool(S, L);
}

int CScriptIniFile::r_token(const char* S, const char* L, const CScriptTokenList &token_list)
{
	return inherited::r_token(S, L, &*token_list.tokens().begin());
}

const char* CScriptIniFile::r_string_wb(const char* S, const char* L)
{
	return *inherited::r_string_wb(S, L);
}

u32	CScriptIniFile::line_count(const char* S)
{
	THROW3(inherited::section_exist(S), "Cannot find section", S);
	return inherited::line_count(S);
}

const char* CScriptIniFile::r_string(const char* S, const char* L)
{
	THROW3(inherited::section_exist(S), "Cannot find section", S);
	THROW3(inherited::line_exist(S, L), "Cannot find line", L);
	return inherited::r_string(S, L);
}

u32	 CScriptIniFile::r_u32(const char* S, const char* L)
{
	THROW3(inherited::section_exist(S), "Cannot find section", S);
	THROW3(inherited::line_exist(S, L), "Cannot find line", L);
	return inherited::r_u32(S, L);
}

int	 CScriptIniFile::r_s32(const char* S, const char* L)
{
	THROW3(inherited::section_exist(S),"Cannot find section", S);
	THROW3(inherited::line_exist(S, L),"Cannot find line", L);
	return inherited::r_s32(S, L);
}

float CScriptIniFile::r_float(const char* S, const char* L)
{
	THROW3(inherited::section_exist(S), "Cannot find section", S);
	THROW3(inherited::line_exist(S, L), "Cannot find line", L);
	return inherited::r_float(S, L);
}

Fvector CScriptIniFile::r_fvector3(const char* S, const char* L)
{
	THROW3(inherited::section_exist(S), "Cannot find section", S);
	THROW3(inherited::line_exist(S, L), "Cannot find line", L);
	return inherited::r_fvector3(S, L);
}

//--------------------------------------------------------------------------------------------------------
// Write functions
//--------------------------------------------------------------------------------------
void CScriptIniFile::w_string(const char* S, const char* L, const char* V)
{
	inherited::w_string(S, L, V);
}

void CScriptIniFile::w_u32(const char* S, const char* L, u32 V)
{
	inherited::w_u32(S, L, V);
}

void CScriptIniFile::w_s32(const char* S, const char* L, s32 V)
{
	inherited::w_s32(S, L, V);
}

void CScriptIniFile::w_float(const char* S, const char* L, float V)
{
	inherited::w_float(S, L, V);
}

void CScriptIniFile::w_bool(const char* S, const char* L, bool V)
{
	inherited::w_bool(S, L, V);
}

void CScriptIniFile::remove_line(const char* S, const char* L)
{
	// TODO mb need THROWS for section and line
	inherited::remove_line(S, L);
}