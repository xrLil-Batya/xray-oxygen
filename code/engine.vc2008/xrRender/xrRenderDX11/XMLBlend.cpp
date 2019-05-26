//////////////////////////////////////////////////////////////
// Desc		: XML Blend Compiler
// Author	: ForserX
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLBlend.h"
#include	"../xrRender/dxRenderDeviceRender.h"

CXMLBlend::CXMLBlend(const char* FileName)
{
	string256 NewName;
	for (int i = 0, l = xr_strlen(FileName) + 1; i < l; ++i)
		NewName[i] = ('\\' == FileName[i]) ? '_' : FileName[i];

	xr_strconcat(NewName, NewName, ".xml");
	File = NewName;

	pCompiler = new CBlender_Compile();
	Parser.Load("$game_shaders$", "r3", File);
	pCompiler->detail_texture = nullptr;
	pCompiler->detail_scaler = nullptr;
}

Shader* CXMLBlend::Compile(const char* Texture)
{
	XML_NODE* pRoot = Parser.GetRoot();
	for (u32 Iter = 0; Iter < 16; Iter++)
	{
		string16 buff;
		xr_sprintf(buff, sizeof(buff), "element_%d", Iter);
		XML_NODE* pElement = Parser.NavigateToNode(pRoot, buff);

		if (pElement)
		{
			dxRenderDeviceRender::Instance().Resources->_ParseList(pCompiler->L_textures, Texture);
			pCompiler->iElement = Iter;
			pCompiler->bDetail = dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(pCompiler->L_textures[0], pCompiler->detail_texture, pCompiler->detail_scaler);

			LocShader.E[Iter] = MakeShader(Texture, pElement);
		}
	}

	for (Shader* pShader : dxRenderDeviceRender::Instance().Resources->v_shaders)
	{
		if (LocShader.equal(pShader))
			return pShader;
	}

	Shader* pShader = xr_new<Shader>(LocShader);
	pShader->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	dxRenderDeviceRender::Instance().Resources->v_shaders.push_back(pShader);
	return pShader;
}

ShaderElement* CXMLBlend::MakeShader(const char* Texture, XML_NODE* pElement)
{
	ShaderElement E;
	pCompiler->SH = &E;
	pCompiler->RS.Invalidate();

	// Compile
	//LPCSTR t_0 = *pCompiler->L_textures[0] ? *pCompiler->L_textures[0] : "null";
	LPCSTR t_1 = (pCompiler->L_textures.size() > 1) ? *pCompiler->L_textures[1] : "null";
	//LPCSTR t_d = pCompiler->detail_texture ? pCompiler->detail_texture : "null";

	// Parse root attributes
	bool bFog = Parser.ReadAttribBool(pElement, "fog");
	bool bZb[2] =
	{
		Parser.ReadAttribBool(pElement, "zb1"),
		Parser.ReadAttribBool(pElement, "zb2"),
	};
	const char* PSName = Parser.ReadAttrib(pElement, "ps", "false");
	const char* VSName = Parser.ReadAttrib(pElement, "vs", "false");
	pCompiler->PassSET_LightFog(FALSE, bFog);
	pCompiler->PassSET_ZB(bZb[0], bZb[1]);
	pCompiler->r_Pass(VSName, PSName, bFog, bZb[0], bZb[1]);

	// Check blend
	XML_NODE* pBlend = Parser.NavigateToNode(pElement, "blend");
	if (pBlend)
	{
		bool bStatus = Parser.ReadAttribBool(pBlend, "status");
		u32 SrcType = BlendValidate(Parser.ReadAttrib(pBlend, "src", "zero"));
		u32 DestType = BlendValidate(Parser.ReadAttrib(pBlend, "dest", "zero"));
		pCompiler->PassSET_ablend_mode(bStatus, SrcType, DestType);
	}

	// Check sorting
	XML_NODE* pSorting = Parser.NavigateToNode(pElement, "sort");
	if (pSorting)
	{
		bool bStatus = Parser.ReadAttribBool(pSorting, "status");
		int Count = Parser.ReadAttribInt(pSorting, "count", 1);
		pCompiler->SetParams(Count, bStatus);
	}

	// Check aref
	XML_NODE* pAref = Parser.NavigateToNode(pElement, "aref");
	if (pAref)
	{
		bool bStatus = Parser.ReadAttribBool(pAref, "status");
		int Count = Parser.ReadAttribInt(pAref, "status", 0);
		pCompiler->PassSET_ablend_aref(bStatus, Count);
	}

	// Check color
	XML_NODE* pColor = Parser.NavigateToNode(pElement, "color");
	if (pColor)
	{
		pCompiler->r_ColorWriteEnable
		(
			Parser.ReadAttribBool(pColor, "r"), 
			Parser.ReadAttribBool(pColor, "g"), 
			Parser.ReadAttribBool(pColor, "b"),
			Parser.ReadAttribBool(pColor, "a")
		);
	}

	// Chec,k body 
	int Idx = 1;
	XML_NODE* pTexture = Parser.NavigateToNode(pElement, "texture");
	do
	{
		shared_str TextureName = Parser.ReadAttrib(pTexture, "name", "none");
		shared_str RtName = Parser.ReadAttrib(pTexture, "rt", "none");
		if (TextureName != "none" && RtName != "none")
		{
			if(TextureName == "t_base")
				pCompiler->r_dx10Texture(t_1, RtName.c_str());
			else
				pCompiler->r_dx10Texture(TextureName.c_str(), RtName.c_str());
		}

		pTexture = Parser.NavigateToNode(pElement, "texture", Idx);
		Idx++;
	} while (pTexture);

	Idx = 1;
	XML_NODE* pSampler = Parser.NavigateToNode(pElement, "sampler");
	do
	{
		shared_str SamplerName = Parser.ReadAttrib(pSampler, "name", "none");
		if (SamplerName != "none")
			pCompiler->r_dx10Sampler(SamplerName.c_str());

		pSampler = Parser.NavigateToNode(pElement, "sampler", Idx);
		Idx++;
	} while (pSampler);

	pCompiler->r_End();
	ShaderElement* pTryElement = dxRenderDeviceRender::Instance().Resources->_CreateElement(E);
	return pTryElement;
}

bool CXMLBlend::Check(const char* FileName)
{
	string256 NewName;
	for (int i = 0, l = xr_strlen(FileName) + 1; i < l; ++i)
		NewName[i] = ('\\' == FileName[i]) ? '_' : FileName[i];

	xr_strconcat(NewName, NewName, ".xml");
	string_path PathAndFile;

	FS.update_path(PathAndFile, "$game_shaders$", "r3\\");
	xr_strconcat(PathAndFile, PathAndFile, NewName);

	return FS.exist(PathAndFile);
}

u32 CXMLBlend::BlendValidate(shared_str type)
{
	u32 SrcType = 0;
	if (type == "zero")			SrcType = 1;
	if (type == "one")	        SrcType = 2;
	if (type == "srccolor")	    SrcType = 3;
	if (type == "invsrccolor")  SrcType = 4;
	if (type == "srcalpha")	    SrcType = 5;
	if (type == "invsrcalpha")  SrcType = 6;
	if (type == "destalpha")	SrcType = 7;
	if (type == "invdestalpha") SrcType = 8;
	if (type == "destcolor")	SrcType = 9;
	if (type == "invdestcolor")	SrcType = 10;
	if (type == "srcalphasat")	SrcType = 11;

	return SrcType;
}
