////////////////////////////////////////////////////////////////////////////
//	Module 		: CameraRecoil.h
//	Created 	: 15.02.2020
//	Author		: ForserX
//	Description : Camera Recoil struct
////////////////////////////////////////////////////////////////////////////
#pragma once

struct SRecoilData
{
	bool		ReturnMode;
	bool		StopReturn;
	
	float		RelaxSpeed;
	float		RelaxSpeed_AI;
	float		MaxAngleVert;
	float		MaxAngleHorz;
	float		Dispersion;
	float		DispersionInc;
	float		DispersionFrac;
	float		StepAngleHorz;
};

//отдача при стрельбе 
struct CameraRecoil
{
	SRecoilData mData;
	
	CameraRecoil()
	{
		mData = 
		{
			false, false,
			EPS_L, EPS_L, 	// Relax speed
			EPS, EPS, 		// Max andle
			EPS, 0.f, 1.f,	// Dispersion
			0.f				// Step angle
		}
	};

	constexpr CameraRecoil(const CameraRecoil& clone )  { mData = clone.mData;	}
	constexpr CameraRecoil(SRecoilData clone) 		    { mData = clone;   		}

	IC void Clone(SRecoilData clone)
	{
		mData = clone;
	}
};
