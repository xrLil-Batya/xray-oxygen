#include "stdafx.h"
#include "xr_effgamma.h"

CGammaControl::CGammaControl() : fGamma(1.0f), fBrightness(1.0f), fContrast(1.0f)
{
	SetBalance(1.0f, 1.0f, 1.0f);
}

void CGammaControl::GetIP(float& G, float &B, float& C, Fvector& Balance)
{
	G = fGamma;
	B = fBrightness;
	C = fContrast;
	Balance.set(cBalance);
}

#ifdef USE_DX11
void CGammaControl::Update() 
{
	if (HW.pDevice) 
	{
		CHK_DX (HW.m_pSwapChain->GetContainingOutput(&pOutput));
		HRESULT hr = pOutput->GetGammaControlCapabilities(&GC);
		if (SUCCEEDED(hr))
		{
			GenLUT();
			pOutput->SetGammaControl(&G);
		}
		_RELEASE(pOutput);
	}
}

void CGammaControl::GenLUT()
{
	DXGI_RGB Offset = {0,0,0};
	DXGI_RGB Scale = {1,1,1};
	G.Offset = Offset;
	G.Scale = Scale;

	float DeltaCV = (GC.MaxConvertedValue - GC.MinConvertedValue);
	
	float og	= 1.f / (fGamma + EPS);
	float B		= fBrightness/2.f;
	float C		= fContrast/2.f;

	for (u32 i=0; i<GC.NumGammaControlPoints; i++) 
	{
		float	c = (C+.5f)*powf( GC.ControlPointPositions[i], og )
					+ (B-0.5f)*0.5f 
					- C*0.5f
					+ 0.25f;

		c = GC.MinConvertedValue + c*DeltaCV;

		G.GammaCurve[i].Red = c*cBalance.x;
		G.GammaCurve[i].Green = c*cBalance.y;
		G.GammaCurve[i].Blue = c*cBalance.z;

		clamp(G.GammaCurve[i].Red, GC.MinConvertedValue, GC.MaxConvertedValue);
		clamp(G.GammaCurve[i].Green, GC.MinConvertedValue, GC.MaxConvertedValue);
		clamp(G.GammaCurve[i].Blue, GC.MinConvertedValue, GC.MaxConvertedValue);
	}
}

#else

IC u16 clr2gamma(float c)
{
	int C = iFloor(c);
	clamp(C, 0, 65535);
	return u16(C);
}

void CGammaControl::Update() 
{
	if (HW.pDevice) 
	{
		GenLUT();
		HW.pDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, &G);
	}
}
void CGammaControl::GenLUT()
{
	float og = 1.f / (fGamma + EPS);
	float B = fBrightness / 2.f;
	float C = fContrast / 2.f;

	for (u32 i = 0; i < 256; i++)
	{
		float c = (C + .5f)*powf(i / 255.f, og)*65535.f + (B - 0.5f)*32768.f - C * 32768.f + 16384.f;
		G.red[i] = clr2gamma(c*cBalance.x);
		G.green[i] = clr2gamma(c*cBalance.y);
		G.blue[i] = clr2gamma(c*cBalance.z);
	}
}
#endif
