#include "stdafx.h"
#include "ExternalCallbackTypes.h"

namespace SpectreCallback 
{
	IClientCallbackDeclaration* pRootNode = nullptr;
	IClientCallbackDeclaration* pCurrentNode = nullptr;

	IClientCallbackDeclaration::IClientCallbackDeclaration(const string64 InName)
		: pNextNode(nullptr)
	{
		memcpy(FunctionName, InName, sizeof(FunctionName));

		if (pRootNode == nullptr)
		{
			pRootNode = this;
		}

		if (pCurrentNode == nullptr)
		{
			pCurrentNode = this;
		}
		else
		{
			pCurrentNode->pNextNode = this;
			pCurrentNode = this;
		}
	}

	DEFINE_CLIENT_CALLBACK_FUNC(shedule_update, u32)
	DEFINE_CLIENT_CALLBACK_FUNC(SecondaryCombine)
}

