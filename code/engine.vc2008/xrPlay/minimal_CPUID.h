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

class CPUID
{
	// forward declarations  
	class CPUID_Internal;

public:
	// Getting the string of cpu vendor and brand (AMD or Intel)
	static std::string Vendor(void)
	{
		return CPU_Rep.vendor_;
	}

	static std::string Brand(void)
	{
		return CPU_Rep.brand_;
	}

	static bool AVX(void)
	{
		return CPU_Rep.f_1_ECX_[28];
	}

	static bool HighEndCPU(void)
	{
		return CPU_Rep.f_1_ECX_[25];		// AES Instruction
	}

private:
	static const CPUID_Internal CPU_Rep;

	class CPUID_Internal
	{
	public:
		CPUID_Internal()
			:
			nIds_{ NULL },
			nExIds_{ NULL },
			isIntel_{},
			isAMD_{},
			f_1_ECX_{ NULL },
			f_1_EDX_{ NULL },
			data_{},
			extdata_{}
		{
			//int cpuInfo[4] = {-1};  
			std::array<int, 4> cpui;

			// Calling __cpuid with 0x0 as the function_id argument  
			// gets the number of the highest valid function ID.  
			__cpuid(cpui.data(), NULL);
			nIds_ = cpui[NULL];

			for (int i = 0; i <= nIds_; ++i)
			{
				__cpuidex(cpui.data(), i, NULL);
				data_.push_back(cpui);
			}

			// Capture vendor string  
			char vendor[0x20];
			memset(vendor, NULL, sizeof(vendor));
			*reinterpret_cast<int*>				(vendor) = data_[0][1];
			*reinterpret_cast<int*>				(vendor + 4) = data_[0][3];
			*reinterpret_cast<int*>				(vendor + 8) = data_[0][2];
			vendor_ = vendor;

			if (vendor_ == "GenuineIntel")
			{
				isIntel_ = true;
				isAMD_ = false;
			}
			else if (vendor_ == "AuthenticAMD")
			{
				isAMD_ = true;
				isIntel_ = false;
			}

			// load bitset with flags for function 0x00000001  
			if (nIds_ >= 1)
			{
				f_1_ECX_ = data_[1][2];
				f_1_EDX_ = data_[1][3];
			}

			// load bitset with flags for function 0x00000007  

			// Calling __cpuid with 0x80000000 as the function_id argument  
			// gets the number of the highest valid extended ID.  


			__cpuid(cpui.data(), 0x80000000);
			nExIds_ = cpui[0];

			char brand[0x40];
			memset(brand, 0, sizeof(brand));

			for (int i = 0x80000000; i <= nExIds_; ++i)
			{
				__cpuidex(cpui.data(), i, 0);
				extdata_.push_back(cpui);
			}

			// load bitset with flags for function 0x80000001  
			// Interpret CPU brand string if reported  
			if (nExIds_ >= 0x80000004)
			{
				memcpy(brand, extdata_[2].data(), sizeof(cpui));
				memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
				memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
				brand_ = brand;
			}

			
		};

		int nIds_;
		int nExIds_;
		std::string vendor_;
		std::string brand_;
		bool isIntel_;
		bool isAMD_;
		std::bitset<32> f_1_ECX_;
		std::bitset<32> f_1_EDX_;
		std::vector<std::array<int, 4>> data_;
		std::vector<std::array<int, 4>> extdata_;
	};
};
