/* file	: buildvm_arch */
// ver	: 0.1

#pragma once
#ifdef _M_X64
#	include "arch_x64.hpp"
#else
#	include "arch_x86.hpp"
#endif