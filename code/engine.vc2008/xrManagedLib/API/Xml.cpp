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

String^ XRay::Xml::Read(String^ node, int index, String^ defaultValue)
{
	string256 NodeName = {};
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(NodeName, index, DefaultValue));
}

String^ XRay::Xml::Read(Node^ startNode, String^ node, int index, String^ defaultValue)
{
	string256 NodeName = {};
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(startNode->pNativeNode, NodeName, index, DefaultValue));
}

String^ XRay::Xml::Read(Node^ node, String^ defaultValue)
{
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->Read(node->pNativeNode, DefaultValue));
}

int XRay::Xml::ReadInt(String^ node, int index, int defaultValue)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return pNativeXml->ReadInt(NodeName, index, defaultValue);
}

int XRay::Xml::ReadInt(Node^ startNode, String^ node, int index, int defaultValue)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return pNativeXml->ReadInt(startNode->pNativeNode, NodeName, index, defaultValue);
}

int XRay::Xml::ReadInt(Node^ node, int defaultValue)
{
	return pNativeXml->ReadInt(node->pNativeNode, defaultValue);
}

float XRay::Xml::ReadFloat(String^ node, int index, float defaultValue)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return pNativeXml->ReadFlt(NodeName, index, defaultValue);
}

float XRay::Xml::ReadFloat(Node^ startNode, String^ node, int index, float defaultValue)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return pNativeXml->ReadFlt(startNode->pNativeNode, NodeName, index, defaultValue);
}

float XRay::Xml::ReadFloat(Node^ node, float defaultValue)
{
	return pNativeXml->ReadFlt(node->pNativeNode, defaultValue);
}

String^ XRay::Xml::ReadAttribute(String^ node, int index, String^ attribute, String^ defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(NodeName, index, Attribute, DefaultValue));
}

String^ XRay::Xml::ReadAttribute(Node^ startNode, String^ node, int index, String^ attribute, String^ defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(startNode->pNativeNode, NodeName, index, Attribute, DefaultValue));
}

String^ XRay::Xml::ReadAttribute(Node^ node, String^ attribute, String^ defaultValue)
{
	string256 Attribute = {};
	string256 DefaultValue = {};

	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(defaultValue, DefaultValue);

	return gcnew String(pNativeXml->ReadAttrib(node->pNativeNode, Attribute, DefaultValue));
}

bool XRay::Xml::ReadAttributeBool(String^ node, int index, String^ attribute, bool defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(NodeName, index, Attribute, defaultValue);
}

bool XRay::Xml::ReadAttributeBool(Node^ startNode, String^ node, int index, String^ attribute, bool defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(startNode->pNativeNode, NodeName, index, Attribute, defaultValue);
}

bool XRay::Xml::ReadAttributeBool(Node^ node, String^ attribute, bool defaultValue)
{
	string256 Attribute = {};

	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribBool(node->pNativeNode, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(String^ node, int index, String^ attribute, int defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(NodeName, index, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(Node^ startNode, String^ node, int index, String^ attribute, int defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(startNode->pNativeNode, NodeName, index, Attribute, defaultValue);
}

int XRay::Xml::ReadAttributeInt(Node^ node, String^ attribute, int defaultValue)
{
	string256 Attribute = {};

	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribInt(node->pNativeNode, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(String^ node, int index, String^ attribute, float defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(NodeName, index, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(Node^ startNode, String^ node, int index, String^ attribute, float defaultValue)
{
	string256 NodeName = {};
	string256 Attribute = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(startNode->pNativeNode, NodeName, index, Attribute, defaultValue);
}

float XRay::Xml::ReadAttributeFloat(Node^ node, String^ attribute, float defaultValue)
{
	string256 Attribute = {};

	ConvertDotNetStringToAscii(attribute, Attribute);

	return pNativeXml->ReadAttribFlt(node->pNativeNode, Attribute, defaultValue);
}

int XRay::Xml::GetNodesCount(String^ node, int index, String^ childNode)
{
	string256 NodeName = {};
	string256 ChildNode = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(childNode, ChildNode);

	return pNativeXml->GetNodesNum(NodeName, index, ChildNode);
}

int XRay::Xml::GetNodesCount(Node^ node, String^ childNode)
{
	string256 ChildNode = {};

	ConvertDotNetStringToAscii(childNode, ChildNode);

	return pNativeXml->GetNodesNum(node->pNativeNode, ChildNode);
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(String^ node, int index)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return gcnew Node(pNativeXml->NavigateToNode(NodeName, index));
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(Node^ startNode, String^ node, int index)
{
	string256 NodeName = {};

	ConvertDotNetStringToAscii(node, NodeName);

	return gcnew Node(pNativeXml->NavigateToNode(startNode->pNativeNode, NodeName, index));
}

XRay::Xml::Node^ XRay::Xml::NavigateToNode(String^ node, String^ attribute, String^ attributeValue)
{
	string256 NodeName = {};
	string256 Attribute = {};
	string256 AttributeValue = {};

	ConvertDotNetStringToAscii(node, NodeName);
	ConvertDotNetStringToAscii(attribute, Attribute);
	ConvertDotNetStringToAscii(attributeValue, AttributeValue);

	return gcnew Node(pNativeXml->NavigateToNodeWithAttribute(NodeName, Attribute, AttributeValue));
}