#include "stdafx.h"
#pragma hdrstop
#include "fs_internal.h"

XRCORE_API CInifile *pSettings = nullptr;

CInifile* CInifile::Create(const char* szFileName, bool ReadOnly) 
{
	return xr_new<CInifile>(szFileName, ReadOnly);
}

void CInifile::Destroy(CInifile* ini) 
{
	xr_delete(ini);
}

XRCORE_API void _parse(LPSTR dest, const char* src) 
{
	if (src) {
		bool bInsideSTR = false;
		while (*src) {
			if (isspace((u8)*src)) {
				if (bInsideSTR) { *dest++ = *src++; continue; }
				while (*src && isspace(*src)) src++;
				continue;
			}
			else if (*src == '"') {
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

XRCORE_API void _decorate(LPSTR dest, const char* src) 
{
	if (src) {
		bool bInsideSTR = false;
		while (*src) {
			if (*src == ',') {
				if (bInsideSTR)
					*dest++ = *src++;
				else {
					*dest++ = *src++;
					*dest++ = ' ';
				}
				continue;
			}
			else if (*src == '"') {
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

bool CInifile::Sect::line_exist(const char* L, const char** val) 
{
	shared_str s(L);
	const auto A = Data.find(s);
	if (A != Data.end()) {
		if (val) *val = *A->second;
		return true;
	}
	return false;
}

CInifile::CInifile(IReader* F, const char* path) 
{
	fName = nullptr;
	bReadOnly = true;
	bSaveAtEnd = false;
	Load(F, path);
}

CInifile::CInifile(const char* szFileName, bool ReadOnly, bool bLoad, bool SaveAtEnd) 
{
	fName = szFileName ? xr_strdup(szFileName) : nullptr;
	bReadOnly = !!ReadOnly;
	bSaveAtEnd = SaveAtEnd;
	if (bLoad) {
		string_path path, folder;
		_splitpath(fName, path, folder, nullptr, nullptr);
		strcat(path, folder);
		IReader* R = FS.r_open(szFileName);
		if (R) {
			Load(R, path);
			FS.r_close(R);
		}
	}
}

CInifile::~CInifile() 
{
	if (!bReadOnly && bSaveAtEnd) {
		if (!save_as())
			Msg("!Can't save inifile:%s", fName);
	}
	if(fName && fName[0])
		xr_free(fName);

	for (std::pair<const shared_str, Sect*>& SectionsLine : DATA)
	{
		xr_delete(SectionsLine.second);
	}
}

static void insert_item(CInifile::Sect *tgt, const CInifile::Item& I) 
{
	auto sect_it = tgt->Data.find(I.first);
	if (sect_it != tgt->Data.end() && sect_it->first.equal(I.first)) {
		sect_it->second = I.second;
		auto found = std::find_if(
			tgt->Unordered.begin(), tgt->Unordered.end(),
			[&](const auto &it) {
			return xr_strcmp(*it.first, *I.first) == 0;
		}
		);
		if (found != tgt->Unordered.end()) {
			found->second = I.second;
		}
	}
	else {
		tgt->Data.insert({ I.first, I.second });
		tgt->Unordered.push_back(I);
	}
}

void CInifile::Load(IReader* F, const char* path) 
{
	R_ASSERT(F);
	Sect       *Current = nullptr;
	string4096 str;
	string4096 str2;

	while (!F->eof()) {
		F->r_string(str, sizeof(str));
		_Trim(str);
		LPSTR semi = strchr(str, ';');
		LPSTR semi_1 = strchr(str, '/');

		if (semi_1 && (*(semi_1 + 1) == '/') && ((!semi) || (semi && (semi_1 < semi))))
			semi = semi_1;

		if (semi)
			*semi = 0;

		if (str[0] && (str[0] == '#') && strstr(str, "#include")) {
			string64 inc_name;
			R_ASSERT(path && path[0]);
			if (_GetItem(str, 1, inc_name, '"')) {
				string_path fn, inc_path, folder;
				xr_strconcat(fn, path, inc_name);
				_splitpath(fn, inc_path, folder, nullptr, nullptr);
				strcat(inc_path, folder);
				IReader* I = FS.r_open(fn);
				R_ASSERT3(I, "Can't find include file:", inc_name);
				Load(I, inc_path);
				FS.r_close(I);
			}
		}
		else if (str[0] && (str[0] == '[')) {
			// insert previous filled section
			if (Current) {
				auto I = DATA.find(Current->Name);
				if (I != DATA.end())
					Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", Current->Name.c_str());
				DATA.insert({ Current->Name, Current });
			}
			Current = xr_new<Sect>();
			Current->Name = nullptr;
			// start new section
			R_ASSERT3(strchr(str, ']'), "Bad ini section found: ", str);
			const char* inherited_names = strstr(str, "]:");
			if (nullptr != inherited_names) {
				VERIFY2(bReadOnly, "Allow for readonly mode only.");
				inherited_names += 2;
				int cnt = _GetItemCount(inherited_names);
				for (int k = 0; k < cnt; ++k) {
					xr_string tmp;
					_GetItem(inherited_names, k, tmp);
					Sect& inherited = r_section(tmp.c_str());
					for (auto &it : inherited.Data) {
						Item I = { it.first, it.second };
						insert_item(Current, I);
					}
				}
			}
			*strchr(str, ']') = 0;
			Current->Name = strlwr(str + 1);
		}
		else {
			if (Current) {
				char* name = str;
				char* t = strchr(name, '=');
				if (t) {
					*t = 0;
					_Trim(name);
					_parse(str2, ++t);
				}
				else {
					_Trim(name);
					str2[0] = 0;
				}

				if (name[0]) {
					Item I;
					I.first = name;
					I.second = str2[0] ? str2 : nullptr;
					if (bReadOnly) {
						if (*I.first)
							insert_item(Current, I);
					}
					else {
						if (*I.first || *I.second)
							insert_item(Current, I);
					}
				}
			}
		}
	}

	if (Current) {
		auto I = DATA.find(Current->Name);
		if (I != DATA.end())
			Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", Current->Name.c_str());
		DATA.insert({ Current->Name, Current });
	}
}

bool CInifile::save_as(const char* new_fname) 
{
	// save if needed
	if (new_fname && new_fname[0]) {
		xr_free(fName);
		fName = xr_strdup(new_fname);
	}
	R_ASSERT(fName && fName[0]);
	IWriter* F = FS.w_open_ex(fName);
	if (F) {
		string512 temp, val;
		for (const auto &r_it : DATA) {
			sprintf_s(temp, sizeof(temp), "[%s]", r_it.first.c_str());
			F->w_string(temp);
			for (const auto &I : r_it.second->Unordered) {
				if (*I.first) {
					if (*I.second) {
						_decorate(val, *I.second);
						{
							// only name and value
							sprintf_s(temp, sizeof(temp), "%8s%-32s = %-32s", " ", *I.first, val);
						}
					}
					else {
						{
							// only name
							sprintf_s(temp, sizeof(temp), "%8s%-32s = ", " ", *I.first);
						}
					}
				}
				else {
					// no name, so no value
					temp[0] = 0;
				}
				_TrimRight(temp);
				if (temp[0]) F->w_string(temp);
			}
			F->w_string(" ");
		}
		FS.w_close(F);
		return true;
	}
	return false;
}

bool CInifile::section_exist(const char* S)
{
	shared_str k(S);
	const auto I = DATA.find(k);
	return I != DATA.end();
}

bool CInifile::line_exist(const char* S, const char* L)
{
	if (!section_exist(S)) return false;
	Sect&   I = r_section(S);
	shared_str k(L);
	const auto A = I.Data.find(k);
	return A != I.Data.end();
}

u32 CInifile::line_count(const char* Sname) 
{
	Sect& S = r_section(Sname);
	return (u32)S.Data.size();
}

CInifile::Sect& CInifile::r_section(const shared_str& S) const
{
	return r_section(S.c_str());
}

bool CInifile::line_exist(const shared_str& S, const shared_str& L) 
{
	return line_exist(S.c_str(), L.c_str());
}

u32 CInifile::line_count(const shared_str& S) 
{
	return line_count(S.c_str());
}

bool CInifile::section_exist(const shared_str& S) 
{
	return section_exist(S.c_str());
}


CInifile::Sect& CInifile::r_section(const char* S) const
{
	R_ASSERT2(S != nullptr, "Empty section passed into CInifile::r_section");

	char section[256];
	strcpy_s(section, sizeof(section), S);
	shared_str k = strlwr(section);
	const auto I = DATA.find(k);
	if (I == DATA.end())
		Debug.fatal(DEBUG_INFO, "Can't open section '%s'", S);
	return  *I->second;
}

const char* CInifile::r_string(const char* S, const char* L) const
{
	R_ASSERT2(S != nullptr, "CInifile::r_string was invoked without section!");
	R_ASSERT3(L != nullptr, "CInifile::r_string was invoked without location!", S);

	VERIFY(xr_strlen(S) > 0);
	VERIFY(xr_strlen(L) > 0);

	Sect&   I = r_section(S);
	shared_str k(L);
	const auto A = I.Data.find(k);
	if (A != I.Data.end())
		return A->second.c_str();
	else
		Debug.fatal(DEBUG_INFO, "Can't find variable %s in [%s]", L, S);
	return nullptr;
}

shared_str CInifile::r_string_wb(const char* S, const char* L) const
{
	const char* _base = r_string(S, L);
	if (nullptr == _base) return  shared_str(nullptr);
	string512 _original;
	strcpy_s(_original, _base);
	u32 _len = xr_strlen(_original);
	if (0 == _len) return  shared_str("");
	if ('"' == _original[_len - 1]) _original[_len - 1] = 0; // skip end
	if ('"' == _original[0]) return  shared_str(&_original[0] + 1); // skip begin
	return shared_str(_original);
}

u8 CInifile::r_u8(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return u8(atoi_17(C));
}

u16 CInifile::r_u16(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return u16(atoi_17(C));
}

u32 CInifile::r_u32(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return u32(atoi_17(C));
}

u64 CInifile::r_u64(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return u64(atoll(C));
}

s8 CInifile::r_s8(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return s8(atoi_17(C));
}

s16 CInifile::r_s16(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return s16(atoi_17(C));
}

s32 CInifile::r_s32(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return s32(atoi_17(C));
}

float CInifile::r_float(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	return float(atof(C));
}

Fcolor CInifile::r_fcolor(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	Fcolor V = { 0, 0, 0, 0 };
	sscanf(C, "%f,%f,%f,%f", &V.r, &V.g, &V.b, &V.a);
	return V;
}

u32 CInifile::r_color(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	u32    r = 0, g = 0, b = 0, a = 255;
	sscanf(C, "%d,%d,%d,%d", &r, &g, &b, &a);
	return color_rgba(r, g, b, a);
}

Ivector2 CInifile::r_ivector2(const char* S, const char* L) const
{
	const char*   C = r_string(S, L);
	Ivector2 V = { 0, 0 };
	sscanf(C, "%d,%d", &V.x, &V.y);
	return V;
}

Ivector3 CInifile::r_ivector3(const char* S, const char* L) const
{
	const char*  C = r_string(S, L);
	Ivector V = { 0, 0, 0 };
	sscanf(C, "%d,%d,%d", &V.x, &V.y, &V.z);
	return V;
}

Ivector4 CInifile::r_ivector4(const char* S, const char* L) const
{
	const char*   C = r_string(S, L);
	Ivector4 V = { 0, 0, 0, 0 };
	sscanf(C, "%d,%d,%d,%d", &V.x, &V.y, &V.z, &V.w);
	return V;
}

Fvector2 CInifile::r_fvector2(const char* S, const char* L) const
{
	const char*   C = r_string(S, L);
	Fvector2 V = { 0.f, 0.f };
	sscanf(C, "%f,%f", &V.x, &V.y);
	return V;
}

Fvector3 CInifile::r_fvector3(const char* S, const char* L) const
{
	const char*   C = r_string(S, L);
	Fvector3 V = { 0.f, 0.f, 0.f };
	sscanf(C, "%f,%f,%f", &V.x, &V.y, &V.z);
	return V;
}

Fvector4 CInifile::r_fvector4(const char* S, const char* L) const
{
	const char*   C = r_string(S, L);
	Fvector4 V = { 0.f, 0.f, 0.f, 0.f };
	sscanf(C, "%f,%f,%f,%f", &V.x, &V.y, &V.z, &V.w);
	return V;
}

bool CInifile::r_bool(const char* S, const char* L) const
{
	const char* C = r_string(S, L);
	char   B[8];
	strncpy(B, C, 7);
	strlwr(B);
	return IsBOOL(B);
}

CLASS_ID CInifile::r_clsid(const char* S, const char* L) 
{
	const char* C = r_string(S, L);
	return TEXT2CLSID(C);
}

int CInifile::r_token(const char* S, const char* L, const xr_token *token_list) 
{
	const char* C = r_string(S, L);
	for (int i = 0; token_list[i].name; i++)
		if (!stricmp(C, token_list[i].name))
			return token_list[i].id;
	return 0;
}

bool CInifile::r_line(const char* S, int L, const char** N, const char** V) 
{
	Sect& SS = r_section(S);
	if (L >= (int)SS.Unordered.size() || L < 0) return false;
	const auto &I = SS.Unordered.at(L);
	*N = I.first.c_str();
	*V = I.second.c_str();
	return true;
}

bool CInifile::r_line(const shared_str& S, int L, const char** N, const char** V) 
{
	return r_line(S.c_str(), L, N, V);
}

void CInifile::w_string(const char* S, const char* L, const char* V) 
{
	R_ASSERT(!bReadOnly);

	// section
	char sect[256];
	_parse(sect, S);
	_strlwr(sect);
	R_ASSERT4(sect[0], "[%s]: wrong section name [%s]", __FUNCTION__, S);
	if (!section_exist(sect)) {
		// create _new_ section
		Sect *NEW = xr_new<Sect>();
		NEW->Name = sect;
		DATA.insert({ NEW->Name, NEW });
	}

	// parse line/value
	char line[256];
	_parse(line, L);
	R_ASSERT4(line[0], "[%s]: wrong param name [%s]", __FUNCTION__, L);
	char value[256];
	_parse(value, V);

	// duplicate & insert
	Item I;
	I.first = line;
	I.second = value[0] ? value : nullptr;
	Sect* data = &r_section(sect);
	insert_item(data, I);
}

void CInifile::w_u8(const char* S, const char* L, u8 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_u16(const char* S, const char* L, u16 V)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_u32(const char* S, const char* L, u32 V)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_u64(const char* S, const char* L, u64 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%I64d", V);
	w_string(S, L, temp);
}

void CInifile::w_s8(const char* S, const char* L, s8 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_s16(const char* S, const char* L, s16 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_s32(const char* S, const char* L, s32 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp);
}

void CInifile::w_float(const char* S, const char* L, float V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f", V);
	w_string(S, L, temp);
}

void CInifile::w_fcolor(const char* S, const char* L, const Fcolor& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.r, V.g, V.b, V.a);
	w_string(S, L, temp);
}

void CInifile::w_color(const char* S, const char* L, u32 V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", color_get_R(V), color_get_G(V), color_get_B(V), color_get_A(V));
	w_string(S, L, temp);
}

void CInifile::w_ivector2(const char* S, const char* L, const Ivector2& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d", V.x, V.y);
	w_string(S, L, temp);
}

void CInifile::w_ivector3(const char* S, const char* L, const Ivector3& V)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d", V.x, V.y, V.z);
	w_string(S, L, temp);
}

void CInifile::w_ivector4(const char* S, const char* L, const Ivector4& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", V.x, V.y, V.z, V.w);
	w_string(S, L, temp);
}

void CInifile::w_fvector2(const char* S, const char* L, const Fvector2& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f", V.x, V.y);
	w_string(S, L, temp);
}

void CInifile::w_fvector3(const char* S, const char* L, const Fvector3& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f", V.x, V.y, V.z);
	w_string(S, L, temp);
}

void CInifile::w_fvector4(const char* S, const char* L, const Fvector4& V) 
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.x, V.y, V.z, V.w);
	w_string(S, L, temp);
}

void CInifile::w_bool(const char* S, const char* L, bool V) 
{
	w_string(S, L, V ? "true" : "false");
}

void CInifile::remove_line(const char* S, const char* L)
{
	R_ASSERT(!bReadOnly);

	if (line_exist(S, L)) {
		Sect& data = r_section(S);
		shared_str k(L);
		auto A = data.Data.find(k);
		R_ASSERT(A != data.Data.end());
		data.Data.erase(A);
		auto found = std::find_if(
			data.Unordered.begin(), data.Unordered.end(),
			[&](const auto& it) {
			return xr_strcmp(*it.first, L) == 0;
		}
		);
		R_ASSERT(found != data.Unordered.end());
		data.Unordered.erase(found);
	}
}

#include <sstream>
xr_string CInifile::get_as_string() 
{
	std::stringstream str;

	bool first_sect = true;
	for (const auto &r_it : DATA) 
	{
		if (!first_sect) str << "\r\n";
		first_sect = false;
		str << "[" << r_it.first.c_str() << "]\r\n";
		for (const auto &I : r_it.second->Unordered) {
			if (I.first.c_str()) {
				if (I.second.c_str()) {
					string512 val;
					_decorate(val, I.second.c_str());
					_TrimRight(val);
					// only name and value
					str << I.first.c_str() << " = " << val << "\r\n";
				}
				else {
					// only name
					str << I.first.c_str() << " =\r\n";
				}
			}
		}
	}

	return xr_string(str.str().c_str());
}

// Uniqle COP
void CInifile::save_as(IWriter& writer, bool bcheck) const 
{
	string4096 temp, val;
	for (auto pairIter : DATA)
	{
		xr_sprintf(temp, sizeof(temp), "[%s]", pairIter.second->Name.c_str());
		writer.w_string(temp);
		if (bcheck) 
		{
			xr_sprintf(temp, sizeof(temp), "; %d %d %d", pairIter.second->Name._get()->dwCRC,
				pairIter.second->Name._get()->dwReference.load(), pairIter.second->Name._get()->dwLength);
			writer.w_string(temp);
		}

		for (Item Itm : pairIter.second->Data)
		{
			if (Itm.first.c_str()) 
			{
				if (Itm.second.c_str()) 
				{
					_decorate(val, Itm.second.c_str());
					// only name and value
					xr_sprintf(temp, sizeof(temp), "%8s%-32s = %-32s", " ", Itm.first.c_str(), val);
				}
				else
				{
					// only name
					xr_sprintf(temp, sizeof(temp), "%8s%-32s = ", " ", Itm.first.c_str());
				}
			}
			else 
			{
				// no name, so no value
				temp[0] = 0;
			}
			_TrimRight(temp);
			if (temp[0])
				writer.w_string(temp);
		}
		writer.w_string(" ");
	}
}
