/*****************************************
* minimal_CPUID.h - Minimal instructions
* for initialize Oxygen's xrPlay.
* Some methods of CPUID maded by Microsoft.
******************************************/
#pragma once
#include <vector>  
#include <bitset>  
#include <array>  
#include <string>  
#include <intrin.h> 
#include <windows.h>

class CPUID {
	// forward declarations  
	class CPUID_Internal;
public:
	static bool AVX			(void)	{ return CPU_Rep.f_1_ECX_[28]; }		// AVX
	static bool SSE41		(void)	{ return CPU_Rep.f_1_ECX_[19]; }		// SSE4.1
private:
	static const CPUID_Internal CPU_Rep;
	class CPUID_Internal {
	public:
		int	nIds_;
		int nExIds_;
		std::bitset<32>	f_1_ECX_;
		std::bitset<32>	f_1_EDX_;
		std::vector<std::array<int, 4>>	data_;
		std::vector<std::array<int, 4>>	extdata_;
		CPUID_Internal() :
			nIds_	{ 0 },
			nExIds_	{ 0 },
			f_1_ECX_{ 0 },
			f_1_EDX_{ 0 },
			data_	{},
			extdata_{} {
			std::array<int, 4> cpui;
			// Calling __cpuid with 0x0 as the function_id argument  
			__cpuid(cpui.data(), NULL);
			nIds_ = cpui[NULL];
			for (int i = 0; i <= nIds_; ++i) 
			{
				__cpuidex(cpui.data(), i, NULL);
				data_.push_back(cpui); 
			}
			// load bitset with flags for function 0x00000001  
			if (nIds_ >= 1) 
			{
				f_1_ECX_ = data_[1][2];
				f_1_EDX_ = data_[1][3]; 
			}
			// Calling __cpuid with 0x80000000 as the function_id argument  
			__cpuid(cpui.data(), 0x80000000);
			nExIds_ = cpui[0];
			// cycle with 0x80000000
			for (int i = 0x80000000; i <= nExIds_; ++i) 
			{
				__cpuidex(cpui.data(), i, 0);
				extdata_.push_back(cpui); 
			}
		};
	};
};