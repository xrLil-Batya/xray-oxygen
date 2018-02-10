// Blender_Vertex.h: interface for the CBlender_LIGHT class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CBlender_LIGHT : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: lighting effect";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);

    virtual		void		Compile(CBlender_Compile& C) {}

	CBlender_LIGHT();
	virtual ~CBlender_LIGHT();
};
