/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 April, 2018
* xrException.h - Exception handler for launcer
* thanks for LostAplhaRus
*************************************************/
#pragma once
/////////////////////////////////////////
#include "xrMain.h"
/////////////////////////////////////////

enum OpSet
{
	SSE3,
	SSE41,
	AVX,
	NO_D3D_DEVICE,
	STRING_NULLPTR,
	MAX_VAL = 0xFFFF
};

class EcxeptionOpSet : public std::exception
{
	private:
	std::string errMessage;
	OpSet opSetDescr			= OpSet::MAX_VAL;
public:
	EcxeptionOpSet(std::string setName, OpSet opSet)
	{
		errMessage					= "Error (", setName, ")";
		opSetDescr					= opSet;
	}
	const char* what			()
		const throw				()
	{
		return errMessage.c_str		();
	}
	OpSet opset() const
	{
		return opSetDescr;
	}
};

