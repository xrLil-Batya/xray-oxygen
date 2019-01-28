#pragma once
#include "../xrCore/XMLCore/xrXMLParser.h"
#include "Filesystem.h"

namespace XRay
{
	public ref class Xml sealed
	{
	internal:
		CXml* pNativeXml;

	public:
		ref class Node sealed
		{
		internal:
			XML_NODE* pNativeNode;

			Node(XML_NODE* node);

		public:
			virtual ~Node();
		};

		property Node^ Root
		{
			virtual Node^ get()
			{
				return gcnew Node(pNativeXml->GetRoot());
			}
		}

		property Node^ LocalRoot
		{
			virtual Node^ get()
			{
				return gcnew Node(pNativeXml->GetLocalRoot());
			}

			virtual void set(Node^ node)
			{
				pNativeXml->SetLocalRoot(node->pNativeNode);
			}
		}

		Xml(String^ path, String^ fileName);
		Xml(String^ pathAlias, String^ path, String^ fileName);

		virtual ~Xml();

		String^ Read(String^ path, int index, String^ defaultValue);
		String^ Read(Node^ startNode, String^ path, int index, String^ defaultValue);
		String^ Read(Node^ node, String^ defaultValue);

		int ReadInt(String^ path, int index, int defaultValue);
		int ReadInt(Node^ startNode, String^ path, int index, int defaultValue);
		int ReadInt(Node^ node, int defaultValue);

		float ReadFloat(String^ path, int index, float defaultValue);
		float ReadFloat(Node^ startNode, String^ path, int index, float defaultValue);
		float ReadFloat(Node^ node, float defaultValue);

		String^ ReadAttribute(String^ path, int index, String^ attribute, String^ defaultValue);
		String^ ReadAttribute(Node^ startNode, String^ path, int index, String^ attribute, String^ defaultValue);
		String^ ReadAttribute(Node^ node, String^ attribute, String^ defaultValue);

		bool ReadAttributeBool(String^ path, int index, String^ attribute, bool defaultValue);
		bool ReadAttributeBool(Node^ startNode, String^ path, int index, String^ attribute, bool defaultValue);
		bool ReadAttributeBool(Node^ node, String^ attribute, bool defaultValue);

		int ReadAttributeInt(String^ path, int index, String^ attribute, int defaultValue);
		int ReadAttributeInt(Node^ startNode, String^ path, int index, String^ attribute, int defaultValue);
		int ReadAttributeInt(Node^ node, String^ attribute, int defaultValue);

		float ReadAttributeFloat(String^ path, int index, String^ attribute, float defaultValue);
		float ReadAttributeFloat(Node^ startNode, String^ path, int index, String^ attribute, float defaultValue);
		float ReadAttributeFloat(Node^ node, String^ attribute, float defaultValue);

		int GetNodesCount(String^ path, int index, String^ tagName);
		int GetNodesCount(Node^ node, String^ tagName);

		Node^ NavigateToNode(String^ path, int index);
		Node^ NavigateToNode(Node^ startNode, String^ path, int index);
		Node^ NavigateToNode(String^ tagName, String^ attribute, String^ attributeValue);
	};
}