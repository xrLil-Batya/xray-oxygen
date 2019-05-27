#pragma once
#include "../../xrCore/XMLCore/xrXMLParser.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"

class CXMLBlend
{
	CBlender_Compile*	pCompiler;
	const char*			File;
	CXml				Parser;
	Shader				LocShader;

public:
					CXMLBlend	(const char* FileName);
	Shader*			Compile		(const char* Texture);
	ShaderElement*	MakeShader	(const char* Texture, XML_NODE* pElement);

static bool			Check		(const char* FileName);

private:
	u32				BlendValidate(shared_str type);
	u32				StencilValidate(shared_str type);
	u32				CMPFunValidate(shared_str type);
};