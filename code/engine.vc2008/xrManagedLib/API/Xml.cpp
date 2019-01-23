#include "stdafx.h"
#include "Xml.h"

/// NODE
XRay::Xml::Node::Node(XML_NODE* node) : pNativeNode(node)
{
}

XRay::Xml::Node::~Node()
{
	pNativeNode->GetDocument()->DeleteNode(pNativeNode);
}

/// XML
XRay::Xml::Xml(String^ path, String^ fileName) : Xml(CONFIG_PATH, path, fileName)
{
}

XRay::Xml::Xml(String^ pathAlias, String^ path, String^ fileName) : pNativeXml(new CXml)
{
	string1024 PathAlias = {};
	string1024 Path = {};
	string256 FileName = {};

	ConvertDotNetStringToAscii(pathAlias, PathAlias);
	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(fileName, FileName);

	pNativeXml->Load(PathAlias, Path, FileName);
}

XRay::Xml::~Xml()
{
	delete pNativeXml;
}

String^ XRay::Xml::Read(String^ path, int index, String^ defaultValue)
{
	string1024 Path = {};
	string1024 DefaultValue = {};

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(Path, index, DefaultValue));
}

String^ XRay::Xml::Read(Node^ startNode, String^ path, int index, String^ defaultValue)
{
	string1024 Path = {};
	string1024 DefaultValue = {};

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(startNode->pNativeNode, Path, index, DefaultValue));
}

String^ XRay::Xml::Read(Node^ node, String^ defaultValue)
{
	string1024 DefaultValue;
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(node->pNativeNode, DefaultValue));
}

int XRay::Xml::ReadInt(String^ path, int index, int defaultValue)
{
	string1024 Path;
	ConvertDotNetStringToAscii(path, Path);

	return pNativeXml->ReadInt(Path, index, defaultValue);
}

int XRay::Xml::ReadInt(Node^ startNode, String^ path, int index, int defaultValue)
{
	string1024 Path = {};
	ConvertDotNetStringToAscii(path, Path);

	return pNativeXml->ReadInt(startNode->pNativeNode, Path, index, defaultValue);
}

int XRay::Xml::ReadInt(Node^ node, int defaultValue)
{
	return pNativeXml->ReadInt(node->pNativeNode, defaultValue);
}

float XRay::Xml::ReadFloat(String^ path, int index, float defaultValue)
{
	string1024 Path;
	ConvertDotNetStringToAscii(path, Path);

	return pNativeXml->ReadFlt(Path, index, defaultValue);
}

float XRay::Xml::ReadFloat(Node^ startNode, String^ path, int index, float defaultValue)
{
	string1024 Path;
	ConvertDotNetStringToAscii(path, Path);

	return pNativeXml->ReadFlt(startNode->pNativeNode, Path, index, defaultValue);
}

float XRay::Xml::ReadFloat(Node^ node, float defaultValue)
{
	return pNativeXml->ReadFlt(node->pNativeNode, defaultValue);
}

String^ XRay::Xml::ReadAttribute(String^ path, int index, String^ attribute, String^ defaultValue)
{
	string1024 Path;
	string256 Attribute;
	string1024 DefaultValue;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(Path, index, Attribute, DefaultValue));
}

String^ XRay::Xml::ReadAttribute(Node^ startNode, String^ path, int index, String^ attribute, String^ defaultValue)
{
	string1024 Path;
	string256 Attribute;
	string1024 DefaultValue;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(startNode->pNativeNode, Path, index, Attribute, DefaultValue));
}

String^ XRay::Xml::ReadAttribute(Node^ node, String^ attribute, String^ defaultValue)
{
	string256 Attribute;
	string1024 DefaultValue;

	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(node->pNativeNode, Attribute, DefaultValue));
}

bool XRay::Xml::ReadAttributeBool(String^ path, int index, String^ attribute, bool defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(Path, index, Attribute, defaultValue);
}

bool XRay::Xml::ReadAttributeBool(Node^ startNode, String^ path, int index, String^ attribute, bool defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(startNode->pNativeNode, Path, index, Attribute, defaultValue);
}

bool XRay::Xml::ReadAttributeBool(Node^ node, String^ attribute, bool defaultValue)
{
	string256 Attribute;
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(node->pNativeNode, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(String^ path, int index, String^ attribute, int defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(Path, index, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(Node^ startNode, String^ path, int index, String^ attribute, int defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(startNode->pNativeNode, Path, index, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(Node^ node, String^ attribute, int defaultValue)
{
	string256 Attribute;
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(node->pNativeNode, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(String^ path, int index, String^ attribute, float defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(Path, index, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(Node^ startNode, String^ path, int index, String^ attribute, float defaultValue)
{
	string1024 Path;
	string256 Attribute;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(startNode->pNativeNode, Path, index, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(Node^ node, String^ attribute, float defaultValue)
{
	string256 Attribute;
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(node->pNativeNode, Attribute, defaultValue);
}

int XRay::Xml::GetNodesCount(String^ path, int index, String^ tagName)
{
	string1024 Path;
	string256 TagName;

	ConvertDotNetStringToAscii(path, Path);
	ConvertDotNetStringToAscii(tagName, TagName);

	return pNativeXml->GetNodesNum(Path, index, TagName);
}

int XRay::Xml::GetNodesCount(Node^ node, String^ tagName)
{
	string256 TagName;
	ConvertDotNetStringToAscii(tagName, TagName);

	return pNativeXml->GetNodesNum(node->pNativeNode, TagName);
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(String^ path, int index)
{
	string1024 Path;
	ConvertDotNetStringToAscii(path, Path);

	return gcnew Node(pNativeXml->NavigateToNode(Path, index));
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(Node^ startNode, String^ path, int index)
{
	string1024 Path;
	ConvertDotNetStringToAscii(path, Path);

	return gcnew Node(pNativeXml->NavigateToNode(startNode->pNativeNode, Path, index));
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(String^ tagName, String^ attribute, String^ attributeValue)
{
	string256 TagName;
	string256 Attribute;
	string256 AttributeValue;

	ConvertDotNetStringToAscii(tagName, TagName);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(attributeValue, AttributeValue);

	return gcnew Node(pNativeXml->NavigateToNodeWithAttribute(TagName, Attribute, AttributeValue));
}