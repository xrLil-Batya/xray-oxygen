#include "stdafx.h"
#pragma hdrstop

#include "xrXMLParser.h"

XRCORE_API CXml::CXml() : m_root(nullptr), m_pLocalRoot(nullptr)
{}

XRCORE_API CXml::~CXml()
{
	ClearInternal();
}

void CXml::ClearInternal()
{
	m_Doc.Clear();
}

inline char* ClearFromWhitespace(char* str)
{
	while (*str == ' ')
		++str;

	return str;
}

void ParseFile(const char* path, CMemoryWriter& W, IReader *F, CXml* xml)
{
	string4096 str;

	while (!F->eof())
	{
		F->r_string(str, sizeof(str));
		char* ClearStr = ClearFromWhitespace(str);

		if (ClearStr[0] == '#' && strstr(ClearStr, "#include"))
		{
			string256 inc_name;
			if (_GetItem(str, 1, inc_name, '"'))
			{
				IReader* I = nullptr;
				if (inc_name == strstr(inc_name, "ui\\"))
				{
					shared_str fn = xml->correct_file_name("ui", strchr(inc_name, '\\') + 1);
					string_path		buff;
					strconcat(sizeof(buff), buff, "ui\\", fn.c_str());
					I = FS.r_open(path, buff);
				}

				if (!I)
				{
					I = FS.r_open(path, inc_name);
					R_ASSERT4(I, "XML file[%s] parsing failed. Can't find include file:[%s]", path, inc_name);
				}

				ParseFile(path, W, I, xml);
				FS.r_close(I);
			}
		}
		else W.w_string(str);
	}
}

void CXml::Load(const char* path_alias, const char* path, const char* _xml_filename)
{
	shared_str fn = correct_file_name(path, _xml_filename);

	string_path str;
	xr_sprintf(str, "%s\\%s", path, *fn);
	return Load(path_alias, str);
}

//инициализаци€ и загрузка XML файла
void CXml::Load(const char* path, const char* xml_filename)
{
	xr_strcpy(m_xml_file_name, xml_filename);
	// Load and parse xml file

	IReader *F = FS.r_open(path, xml_filename);
	R_ASSERT2(F, xml_filename);

	CMemoryWriter W;
	ParseFile(path, W, F, this);
	W.w_stringZ("");
	FS.r_close(F);

	m_Doc.Parse((const char*)W.pointer());

	if (m_Doc.Error())
	{
        string4096 ErrorInfo = { 0 };
        m_Doc.DumpError(ErrorInfo, xml_filename);

        Debug.fatal(DEBUG_INFO, ErrorInfo);
	}

	m_root = m_Doc.FirstChildElement();
}

XML_NODE* CXml::NavigateToNode(XML_NODE* start_node, const char*  path, int node_index)
{
	R_ASSERT3(start_node && path, "NavigateToNode failed in XML file ", m_xml_file_name);
	XML_NODE* node = nullptr;

	string_path buf_str;
	buf_str[0] = 0;
	xr_strcpy(buf_str, path);

	char seps[] = ":";
	char *token;

	//разбить путь на отдельные подпути
	token = strtok(buf_str, seps);

	if (token)
	{
		node = start_node->FirstChildElement(token);

		for (int tmp = 0; tmp < node_index && node; tmp++)
		{
			//node = start_node->IterateChildren(token, node);
			//FX: tinyxml::IterateChildren code:
			if (node)
			{
				R_ASSERT(node->Parent() == start_node);
				node = node->NextSiblingElement(token);
			}
			else
			{
				node = start_node->FirstChildElement(token);
			}
		}
	}

	while (token)
	{
		// Get next token:
		token = strtok(nullptr, seps);

		if (token && node)
		{
			XML_NODE* node_parent = node;
			node = node_parent->FirstChildElement(token);
		}
	}

	return node;
}

XML_NODE* CXml::NavigateToNode(const char* path, int node_index)
{
	return NavigateToNode(GetLocalRoot() ? GetLocalRoot() : GetRoot(), path, node_index);
}

XML_NODE* CXml::NavigateToNodeWithAttribute(const char* tag_name, const char* attrib_name, const char* attrib_value)
{
	XML_NODE *root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
	int tabsCount = GetNodesNum(root, tag_name);

	for (int i = 0; i < tabsCount; ++i)
	{
		const char* result = ReadAttrib(root, tag_name, i, attrib_name, "");
		if (result && xr_strcmp(result, attrib_value) == 0)
		{
			return NavigateToNode(root, tag_name, i);
		}
	}
	return nullptr;
}

const char* CXml::Read(const char* path, int index, const char* default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	return					Read(node, default_str_val);
}

const char* CXml::Read(XML_NODE* start_node, const char* path, int index, const char* default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	return					Read(node, default_str_val);
}

const char*  CXml::Read(XML_NODE* node, const char* default_str_val)
{
	if (node)
	{
		node = node->FirstChild();
		if (node)
		{
			tinyxml2::XMLText *text = node->ToText();
			if (text)
				return text->Value();
		}
	}
	return default_str_val;
}

int CXml::ReadInt(XML_NODE* node, int default_int_val)
{
	const char* result_str = Read(node, nullptr);

	return result_str ? atoi(result_str) : default_int_val;
}

int CXml::ReadInt(const char* path, int index, int default_int_val)
{
	const char* result_str = Read(path, index, nullptr);
	return result_str ? atoi(result_str) : default_int_val;
}

int CXml::ReadInt(XML_NODE* start_node, const char* path, int index, int default_int_val)
{
	const char* result_str = Read(start_node, path, index, nullptr);
	return result_str ? atoi(result_str) : default_int_val;
}

float CXml::ReadFlt(const char* path, int index, float default_flt_val)
{
	const char* result_str = Read(path, index, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

float CXml::ReadFlt(XML_NODE* start_node, const char* path, int index, float default_flt_val)
{
	const char* result_str = Read(start_node, path, index, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

float CXml::ReadFlt(XML_NODE* node, float default_flt_val)
{
	const char* result_str = Read(node, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

const char* CXml::ReadAttrib(XML_NODE* start_node, const char* path, int index, const char* attrib, const char* default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	return ReadAttrib(node, attrib, default_str_val);
}

const char* CXml::ReadAttrib(const char* path, int index, const char* attrib, const char* default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	return ReadAttrib(node, attrib, default_str_val);
}

const char* CXml::ReadAttrib(XML_NODE* node, const char* attrib, const char* default_str_val)
{
	if (node)
	{
		/*
				//об€зательно делаем ref_str, а то
				//не сможем запомнить строку и return вернет левый указатель
				shared_str result_str;
		*/
		const char* result_str;
		//  астаем ниже по иерархии

		tinyxml2::XMLElement *el = node->ToElement();

		if (el)
		{
			result_str = el->Attribute(attrib);
			if (result_str)
				return result_str;
		}
	}
	return default_str_val;
}

int CXml::ReadAttribInt(XML_NODE* node, const char* attrib, int default_int_val)
{
	const char* result_str = ReadAttrib(node, attrib, nullptr);
	return result_str ? atoi(result_str) : default_int_val;
}

int CXml::ReadAttribInt(const char* path, int index, const char* attrib, int default_int_val)
{
	const char* result_str = ReadAttrib(path, index, attrib, nullptr);
	return result_str ? atoi(result_str) : default_int_val;
}

int CXml::ReadAttribInt(XML_NODE* start_node, const char* path, int index, const char* attrib, int default_int_val)
{
	const char* result_str = ReadAttrib(start_node, path, index, attrib, nullptr);
	return result_str ? atoi(result_str) : default_int_val;
}

float CXml::ReadAttribFlt(const char* path, int index, const char* attrib, float default_flt_val)
{
	const char* result_str = ReadAttrib(path, index, attrib, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

float CXml::ReadAttribFlt(XML_NODE* start_node, const char* path, int index, const char* attrib, float default_flt_val)
{
	const char* result_str = ReadAttrib(start_node, path, index, attrib, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

float CXml::ReadAttribFlt(XML_NODE* node, const char* attrib, float default_flt_val)
{
	const char* result_str = ReadAttrib(node, attrib, nullptr);
	return result_str ? (float)atof(result_str) : default_flt_val;
}

int CXml::GetNodesNum(const char* path, int index, const char* tag_name)
{
	XML_NODE *node;
	XML_NODE *root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
	if (path)
	{
		node = NavigateToNode(path, index);
		if (!node)
		{
			node = root;
		}
	}
	else node = root;

	return node ? GetNodesNum(node, tag_name) : 0;
}

int CXml::GetNodesNum(XML_NODE* node, const char*  tag_name)
{
	int result = 0;
	if (node)
	{
		XML_NODE *el;

		if (!tag_name)
			el = node->FirstChild();
		else
			el = node->FirstChildElement(tag_name);

		while (el)
		{
			++result;
			if (!tag_name)
				el = el->NextSibling();
			else
				el = el->NextSiblingElement(tag_name);
		}
	}
	return result;
}

//нахождение элемнета по его атрибуту
XML_NODE* CXml::SearchForAttribute(const char* path, int index, const char* tag_name, const char* attrib, const char* attrib_value_pattern)
{
	XML_NODE* start_node = NavigateToNode(path, index);
	return	SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
}

XML_NODE* CXml::SearchForAttribute(XML_NODE* start_node, const char* tag_name, const char* attrib, const char* attrib_value_pattern)
{
	while (start_node)
	{
		tinyxml2::XMLElement *el = start_node->ToElement();
		if (el)
		{
			const char* attribStr = el->Attribute(attrib);
			const char* valueStr = el->Value();

			if (attribStr && !xr_strcmp(attribStr, attrib_value_pattern) && valueStr && !xr_strcmp(valueStr, tag_name))
			{
				return el;
			}
		}

		XML_NODE *newEl = start_node->FirstChildElement(tag_name);
		newEl = SearchForAttribute(newEl, tag_name, attrib, attrib_value_pattern);
		if (newEl)
			return newEl;

		start_node = start_node->NextSiblingElement(tag_name);
	}
	return nullptr;
}

const char* CXml::CheckUniqueAttrib(XML_NODE* start_node, const char* tag_name, const char* attrib_name)
{
	m_AttribValues.clear();

	const u32 tags_num = GetNodesNum(start_node, tag_name);

	for (u32 i = 0; i < tags_num; i++)
	{
		const char* attrib = ReadAttrib(start_node, tag_name, i, attrib_name, nullptr);
		auto it = std::find(m_AttribValues.begin(), m_AttribValues.end(), attrib);
		if (m_AttribValues.end() != it)
			return attrib;

		m_AttribValues.push_back(attrib);
	}
	return nullptr;
}