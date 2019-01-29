#pragma once
#include "../xrCore/XMLCore/xrXMLParser.h"
#include "Filesystem.h"

namespace XRay
{
	public ref class Xml 
	{
	internal:
		CXml* pNativeXml;

	public:
		ref class Node
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

		String^ Read(String^ node, int index, String^ defaultValue);
		String^ Read(Node^ startNode, String^ node, int index, String^ defaultValue);
		String^ Read(Node^ node, String^ defaultValue);

		int ReadInt(String^ node, int index, int defaultValue);
		int ReadInt(Node^ startNode, String^ node, int index, int defaultValue);
		int ReadInt(Node^ node, int defaultValue);

		float ReadFloat(String^ node, int index, float defaultValue);
		float ReadFloat(Node^ startNode, String^ node, int index, float defaultValue);
		float ReadFloat(Node^ node, float defaultValue);

		String^ ReadAttribute(String^ node, int index, String^ attribute, String^ defaultValue);
		String^ ReadAttribute(Node^ startNode, String^ node, int index, String^ attribute, String^ defaultValue);
		String^ ReadAttribute(Node^ node, String^ attribute, String^ defaultValue);

		bool ReadAttributeBool(String^ node, int index, String^ attribute, bool defaultValue);
		bool ReadAttributeBool(Node^ startNode, String^ node, int index, String^ attribute, bool defaultValue);
		bool ReadAttributeBool(Node^ node, String^ attribute, bool defaultValue);

		int ReadAttributeInt(String^ node, int index, String^ attribute, int defaultValue);
		int ReadAttributeInt(Node^ startNode, String^ node, int index, String^ attribute, int defaultValue);
		int ReadAttributeInt(Node^ node, String^ attribute, int defaultValue);

		float ReadAttributeFloat(String^ node, int index, String^ attribute, float defaultValue);
		float ReadAttributeFloat(Node^ startNode, String^ node, int index, String^ attribute, float defaultValue);
		float ReadAttributeFloat(Node^ node, String^ attribute, float defaultValue);

		int GetNodesCount(String^ node, int index, String^ childNode);
		int GetNodesCount(Node^ node, String^ childNode);

		Node^ NavigateToNode(String^ node, int index);
		Node^ NavigateToNode(Node^ startNode, String^ node, int index);
		Node^ NavigateToNode(String^ node, String^ attribute, String^ attributeValue);
	};
}