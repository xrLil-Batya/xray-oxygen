/*****************************************
* minimal_CPUID.cpp - Minimal instructions
* for initialize Oxygen's xrPlay.
* Some methods of CPUID maded by Microsoft.
*
******************************************/
#include "minimal_CPUID.h"

const CPUID::CPUID_Internal CPUID::CPU_Rep;

// Main method for testing
/*int main()
{
	auto& outstream = std::cout;

	/*auto support_message = [&outstream](std::string isa_feature, bool is_supported) {
		outstream << isa_feature << (is_supported ? " supported" : " not supported") << std::endl;
	};
	
	std::cout << InstructionSet::Vendor		( ) << std::endl;
	std::cout << InstructionSet::Brand		( ) << std::endl;

	//support_message("AVX",	InstructionSet::AVX		());
	//support_message("RDTSCP", InstructionSet::RDTSCP	());
}
*/