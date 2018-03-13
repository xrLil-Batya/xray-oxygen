#pragma once

class CBlender_droplets : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "Droplets";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_droplets();
	virtual ~CBlender_droplets();
};
