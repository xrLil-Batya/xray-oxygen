#pragma once

enum class CPUFeature: unsigned
{
	MMX				= 1 << 0,
	MMXExt			= 1 << 1,
	MWait			= 1 << 2,
	HT				= 1 << 3,

	SSE				= 1 << 4,
	SSE2			= 1 << 5,
	SSE3			= 1 << 6,
	SSSE3			= 1 << 7,
	SSE41			= 1 << 8,
	SSE4a			= 1 << 9,
	SSE42			= 1 << 10,

	AVX				= 1 << 11,
	AVX2			= 1 << 12,
	AVX512F			= 1 << 13,
	AVX512PF		= 1 << 14,
	AVX512ER		= 1 << 15,
	AVX512CD		= 1 << 16,

	AMD_3DNow		= 1 << 17,
	AMD_3DNowExt	= 1 << 18
};

struct processor_info 
{
	processor_info();

	unsigned char family;	// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
	unsigned char model;	// model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
	unsigned char stepping; // Processor revision number

	bool isAmd;				// AMD flag
	bool isIntel;			// IntelCore flag
	char vendor[32];
	char modelName[64];

	unsigned features;		// processor Feature ( same as return value).

	unsigned n_cores;		// number of available physical cores
	unsigned n_threads;		// number of available logical threads

	unsigned affinity_mask; // recommended affinity mask
								// all processors available to process
								// except 2nd (and upper) logical threads
								// of the same physical core

	bool hasFeature(const CPUFeature feature) const noexcept
	{
		return (features & static_cast<unsigned>(feature));
	}

};