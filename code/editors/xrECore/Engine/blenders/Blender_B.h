// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CBlender_B : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: Implicit";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);

    virtual		void		Compile(CBlender_Compile& C) {}

	CBlender_B();
	virtual ~CBlender_B();
};
