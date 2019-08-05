//////////////////////////////////////////////////////////////////////////
// string_table.h:		таблица строк используемых в игре
//////////////////////////////////////////////////////////////////////////
#pragma once

enum EGameLanguage
{
	eglRussian = 0,
	eglEnglish = 1
};

extern xr_token language_type_token[];

extern EGameLanguage g_Language;

using STRING_TABLE_MAP = xr_map<shared_str, shared_str>;

struct STRING_TABLE_DATA
{
	shared_str				m_sLanguage;
	STRING_TABLE_MAP		m_StringTable;
	STRING_TABLE_MAP		m_string_key_binding;
};

class ENGINE_API CStringTable
{
public:
								CStringTable			();

	static void					Destroy					();
	
	shared_str					translate				(const shared_str& str_id)		const;
			void				ReInit					(EGameLanguage lang);
			void				rescan					();

	static	void				ReparseKeyBindings		();
private:
			void				Init					();
			void				Load					(LPCSTR xml_file);
	static shared_str			ParseLine				(LPCSTR str, LPCSTR key, bool bFirst);
	static STRING_TABLE_DATA*	pData;
};
