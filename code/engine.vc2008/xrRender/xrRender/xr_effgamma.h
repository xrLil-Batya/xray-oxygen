#pragma once

class CGammaControl
{
private:
	float	fGamma;
	float	fBrightness;
	float	fContrast;
	Fvector	cBalance;

public:
				CGammaControl	();

	IC  Fvector	GetBalance		() const						{ return cBalance; }
	IC	void	SetBalance		(float r, float g, float b)		{ cBalance.set(r, g, b); }
	IC	void	SetBalance		(Fvector &C)					{ SetBalance(C.x, C.y, C.z); }

	IC	float	GetGamma		() const						{ return fGamma; }
	IC	float	GetBrightness	() const						{ return fBrightness; }
	IC	float	GetContrast		() const						{ return fContrast; }
	IC	void	SetGamma		(float G)						{ fGamma = G; }
	IC	void	SetBrightness	(float B)						{ fBrightness = B; }
	IC	void	SetContrast		(float C)						{ fContrast	= C; }

		void	GetIP			(float& G, float &B, float& C, Fvector& Balance);
		void	Update			();

private:
#if defined(USE_DX10) || defined(USE_DX11)
		void	GenLUT			(const DXGI_GAMMA_CONTROL_CAPABILITIES &GC, DXGI_GAMMA_CONTROL &G);
#else	//	USE_DX10
		void	GenLUT			(D3DGAMMARAMP &G);
#endif	//	USE_DX10
};
