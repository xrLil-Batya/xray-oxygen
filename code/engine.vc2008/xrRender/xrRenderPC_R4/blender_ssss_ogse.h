#pragma once

class CBlender_ssss_ogse : public IBlender
{
public:
    virtual	LPCSTR		getComment		() { return "OGSE: sunshafts"; }
    virtual	BOOL		canBeDetailed	() { return FALSE; }
    virtual	BOOL		canBeLMAPped	() { return FALSE; }

    virtual	void		Compile			(CBlender_Compile& C);

						CBlender_ssss_ogse	();
    virtual				~CBlender_ssss_ogse	();
};