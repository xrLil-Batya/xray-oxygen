#include "stdafx.h"
#include "xr_effgamma.h"

CGammaControl::CGammaControl() : fGamma(1.0f), fBrightness(1.0f), fContrast(1.0f)
{
	SetBalance(1.0f, 1.0f, 1.0f);
}

void CGammaControl::GetIP(float& refG, float &refB, float& refC, Fvector& refBalance)
{
	refG = fGamma;
	refB = fBrightness;
	refC = fContrast;
	refBalance.set(cBalance);
}

void CGammaControl::Update() 
{
	if (HW.pDevice)
	{
		HW.m_pSwapChain->GetContainingOutput(&pOutput);
		if (pOutput)
		{
			const HRESULT& hr = pOutput->GetGammaControlCapabilities(&GC);
			if (SUCCEEDED(hr))
			{
				GenLUT();
				pOutput->SetGammaControl(&G);
			}
			_RELEASE(pOutput);
		}
	}
}

void CGammaControl::GenLUT()
{
	G.Offset = { 0,0,0 };
	G.Scale  = { 1,1,1 };

	float DeltaCV = (GC.MaxConvertedValue - GC.MinConvertedValue);

	float og = 1.f / (fGamma + EPS);
	float B  = fBrightness / 2.f;
	float C  = fContrast / 2.f;

	for (u32 i = 0; i < GC.NumGammaControlPoints; i++)
	{
		float	c = (C + .5f) * powf(GC.ControlPointPositions[i], og) + (B - 0.5f) * 0.5f - C * 0.5f + 0.25f;

		c = GC.MinConvertedValue + c * DeltaCV;

		G.GammaCurve[i].Red = c * cBalance.x;
		G.GammaCurve[i].Green = c * cBalance.y;
		G.GammaCurve[i].Blue = c * cBalance.z;

		clamp(G.GammaCurve[i].Red, GC.MinConvertedValue, GC.MaxConvertedValue);
		clamp(G.GammaCurve[i].Green, GC.MinConvertedValue, GC.MaxConvertedValue);
		clamp(G.GammaCurve[i].Blue, GC.MinConvertedValue, GC.MaxConvertedValue);
	}
}