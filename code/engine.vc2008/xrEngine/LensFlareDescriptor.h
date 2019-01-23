#pragma once

class ENGINE_API CLensFlareDescriptor
{
public:
	struct SFlare
	{
    	float		fOpacity;
	    float		fRadius;
    	float		fPosition;
        shared_str	texture;
        shared_str	shader;
		FactoryPtr<IFlareRender>m_pRender;
    	SFlare()
		{ 
			fOpacity = fRadius = fPosition = 0; 
		}
	};
    struct SSource: public SFlare
    {
    	BOOL			ignore_color;
    };

	xr_vector<SFlare>	m_Flares;

	enum
	{
    	flFlare 		= (1<<0),
    	flSource		= (1<<1),
    	flGradient 		= (1<<2)
    };
	Flags32				m_Flags;
    
	// source
    SSource				m_Source;
    
	// gradient
    SFlare				m_Gradient;

    float				m_StateBlendUpSpeed;
    float				m_StateBlendDnSpeed;
    
	void				SetGradient		(float fMaxRadius, float fOpacity, LPCSTR tex_name, LPCSTR sh_name);
    void				SetSource		(float fRadius, BOOL ign_color, LPCSTR tex_name, LPCSTR sh_name);
    void				AddFlare		(float fRadius, float fOpacity, float fPosition, LPCSTR tex_name, LPCSTR sh_name);

	shared_str			section;
public:
    					CLensFlareDescriptor();
    void				Load			(CInifile* pIni, LPCSTR section);
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();
};
