////////////////////////////////////////////////////
// VERTVER, 2018 (C) * X-RAY OXYGEN 1.7 PROJECT	
// minimal_CPUID.h - MinCPUID for OXYLaunch		
// Edited: 26 March, 2018						
////////////////////////////////////////////////////
#pragma once								
////////////////////////////////////////////////////
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>	
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//#VERTVER: What's this? OwO
//
//#First_role: xrPlay CPUID isn't xrCore CPUID.
//#Second_role: use void. Always.
//#HowToUse: 
//if (CPUID::AMD || CPUID::AMDelse) { printf("AMD"); }
//
////////////////////////////////////////////////////
class CPUID {
	// forward declarations  
	class CPUID_Internal;
public:
	////////////////////////////////////////////////////
	static bool SSE2		(void)	{ return CPU_Rep.f_1_EDX_[26]; }		// SSE2
	static bool SSE3		(void)	{ return CPU_Rep.f_1_ECX_[0];  }		// SSE3
	static bool SSE41		(void)	{ return CPU_Rep.f_1_ECX_[19]; }		// SSE4.1
	static bool AVX			(void)	{ return CPU_Rep.f_1_ECX_[28]; }		// AVX
	////////////////////////////////////////////////////
	//#VERTVER: HACK: AMD only istructions supported only on AMD
	static bool AMD			(void)	{ return CPU_Rep.f_1_ECX_[6];  }		// SSE4a
	static bool AMDelse		(void)	{ return CPU_Rep.f_81_EDX_[31];}		// 3DNow!
	////////////////////////////////////////////////////

private:
	static const CPUID_Internal CPU_Rep;
	class CPUID_Internal {
	public:
		/////////////////////////////////////////
		int	nIds_;
		int nExIds_;
		std::bitset<32>	f_1_ECX_;
		std::bitset<32>	f_1_EDX_;
		std::bitset<32> f_81_ECX_;
		std::bitset<32> f_81_EDX_;
		std::vector<std::array<int, 4>>data_;
		std::vector<std::array<int, 4>>extdata_;
		/////////////////////////////////////////

		// Create the main class of CPUID
		/////////////////////////////////////////
		CPUID_Internal() :
			nIds_	 { 0 },
			nExIds_	 { 0 },
			f_1_ECX_ { 0 },
			f_1_EDX_ { 0 },
			f_81_ECX_{ 0 },
			f_81_EDX_{ 0 },
			data_	 {},
			extdata_ {}

		{
			std::array<int, 4> cpui;

			// Calling __cpuid with 0x0 as the function_id argument  
			/////////////////////////////////////////
			__cpuid(cpui.data(), 0);
			nIds_ = cpui[0];
			/////////////////////////////////////////

			for (int i = 0; i <= nIds_; ++i)
			{
				__cpuidex(cpui.data(), i, 0);
				data_.push_back(cpui);
			}

			// load bitset with flags for function 0x00000001  
			/////////////////////////////////////////
			if (nIds_ >= 1)
			{
				f_1_ECX_ = data_[1][2];
				f_1_EDX_ = data_[1][3];
			}

			// Calling __cpuid with 0x80000000 as the function_id argument  
			/////////////////////////////////////////  
			__cpuid(cpui.data(), 0x80000000);
			nExIds_ = cpui[0];
			/////////////////////////////////////////

			// cycle with 0x80000000
			/////////////////////////////////////////
			for (int i = 0x80000000; i <= nExIds_; ++i)
			{
				__cpuidex(cpui.data(), i, 0);
				extdata_.push_back(cpui);
			}

			// load bitset with flags for function 0x00000001  
			/////////////////////////////////////////
			if (nExIds_ >= 0x80000001)
			{
				f_81_ECX_ = extdata_[1][2];
				f_81_EDX_ = extdata_[1][3];
			}
		};
		/////////////////////////////////////////
	};
};