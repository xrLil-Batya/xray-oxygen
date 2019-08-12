#include "stdafx.h"
#include "string_table.h"

#include "../xrCore/XMLCore/xrXMLParser.h"
#include "xr_level_controller.h"

STRING_TABLE_DATA* CStringTable::pData = nullptr;

EGameLanguage g_Language = EGameLanguage::eglRussian;

xr_token language_type_token[] =
{
	{ "Русский", 0  },
	{ "English", 1  },
	{ nullptr,	 2	} // required
};

CStringTable::CStringTable	()
{
	Init();
}

void CStringTable::Destroy	()
{
	xr_delete(pData);
}

void CStringTable::rescan()
{
	if(nullptr != pData)	return;
	Destroy				();
	Init				();
}

void CStringTable::Init		()
{
	if(nullptr != pData) return;
    
	pData = xr_new<STRING_TABLE_DATA>();
	
	if (g_Language == EGameLanguage::eglEnglish)
	{
		pData->m_sLanguage = "eng";
	}
	else if (g_Language == EGameLanguage::eglRussian)
	{
		pData->m_sLanguage = "rus";
	}

//---
	FS_FileSet fset;
	string_path files_mask;
	xr_sprintf(files_mask, "text\\%s\\*.xml",pData->m_sLanguage.c_str());
	FS.file_list(fset, "$game_config$", FS_ListFiles, files_mask);

	for (FS_File File : fset)
	{
		string_path fn, ext;
		_splitpath(File.name.c_str(), nullptr, nullptr, fn, ext);
		xr_strcat(fn, ext);

		Load(fn);
	}
//---
	ReparseKeyBindings();
}

void CStringTable::ReInit(EGameLanguage lang)
{
	if (lang == g_Language) return;
	if (pData != nullptr)
	{
		Destroy();
	}
	pData = xr_new<STRING_TABLE_DATA>();

	LPCSTR languageStr = nullptr;

	switch (lang)
	{
	case eglRussian:
		languageStr = "rus";
		break;
	case eglEnglish:
		languageStr = "eng";
		break;
	default:
		VERIFY2(false, "Unsupported language!");
		break;
	}
	pData->m_sLanguage = languageStr;

	FS_FileSet fset;
	string_path			files_mask;
	xr_sprintf(files_mask, "text\\%s\\*.xml", languageStr);
	FS.file_list(fset, "$game_config$", FS_ListFiles, files_mask);

	for (FS_File File: fset)
	{
		string_path fn, ext;
		_splitpath(File.name.c_str(), nullptr, nullptr, fn, ext);
		xr_strcat(fn, ext);

		Load(fn);
	}

	ReparseKeyBindings();
}

void CStringTable::Load	(LPCSTR xml_file_full)
{
	CXml uiXml;
	string_path _s;
	xr_strconcat(_s, "text\\", pData->m_sLanguage.c_str() );

	uiXml.Load(CONFIG_PATH, _s, xml_file_full);

	//РѕР±С‰РёР№ СЃРїРёСЃРѕРє РІСЃРµС… Р·Р°РїРёСЃРµР№ С‚Р°Р±Р»РёС†С‹ РІ С„Р°Р№Р»Рµ
	int string_num = uiXml.GetNodesNum(uiXml.GetRoot(), "string");

	for(int i=0; i<string_num; ++i)
	{
		LPCSTR string_name = uiXml.ReadAttrib(uiXml.GetRoot(), "string", i, "id", nullptr);

		VERIFY4(pData->m_StringTable.find(string_name) == pData->m_StringTable.end(), "duplicate string table id", string_name, uiXml.m_xml_file_name);

		LPCSTR string_text		= uiXml.Read(uiXml.GetRoot(), "string:text", i,  nullptr);
		shared_str str_val		= ParseLine(string_text, string_name, true);
		pData->m_StringTable[string_name] = str_val;
	}
}

void CStringTable::ReparseKeyBindings()
{
	if(!pData)					return;
    auto it		= pData->m_string_key_binding.begin();
    auto it_e	= pData->m_string_key_binding.end();

	for(;it!=it_e;++it)
	{
		pData->m_StringTable[it->first]			= ParseLine(*it->second, *it->first, false);
	}
}


shared_str CStringTable::ParseLine(LPCSTR str, LPCSTR skey, bool bFirst)
{
//	LPCSTR str = "1 $$action_left$$ 2 $$action_right$$ 3 $$action_left$$ 4";
    if (str == nullptr)
    {
        //doesn't have localization info, display as key
        return shared_str(skey);
    }

	xr_string			res;
	int k = 0;
	const char*			b;
	#define ACTION_STR "$$ACTION_"

//.	int LEN				= (int)xr_strlen(ACTION_STR);
	#define LEN			9

	string256				buff;
	string256				srcbuff;
	bool	b_hit			= false;

	while( (b = strstr( str+k,ACTION_STR)) !=nullptr )
	{
		buff[0]				= 0;
		srcbuff[0]			= 0;
		res.append			(str+k, b-str-k);
		const char* e		= strstr( b+LEN,"$$" );

		int len				= (int)(e-b-LEN);

		strncpy_s				(srcbuff,b+LEN, len);
		srcbuff[len]		= 0;
		GetActionAllBinding	(srcbuff, buff, sizeof(buff) );
		res.append			(buff, xr_strlen(buff) );

		k					= (int)(b-str);
		k					+= len;
		k					+= LEN;
		k					+= 2;
		b_hit				= true;
	};

	if(k<(int)xr_strlen(str)){
		res.append(str+k);
	}

	if(b_hit&&bFirst) pData->m_string_key_binding[skey] = str;

	return shared_str(res.c_str());
}

shared_str CStringTable::translate (const shared_str& str_id) const
{
	VERIFY					(pData);

	if(pData->m_StringTable.find(str_id)!=pData->m_StringTable.end())
		return  pData->m_StringTable[str_id];
	else
		return str_id;
}
