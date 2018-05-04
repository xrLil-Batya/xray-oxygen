#ifndef _LOCAL_RAND
#define _LOCAL_RAND

/*
u32 dwRandSeed;
IC u32 dwfRandom(u32 dwRange)
{
	u32 dwResult;
	__asm {
		mov     eax,dwRange						_eax		= dwRange
		imul    edx,dwRandSeed,08088405H
		inc     edx
		mov     dwRandSeed,edx					dwRandSeed	= (dwRandSeed * 08088405H)+1
		mul     edx								return		(u64(dwRange) * u64(dwRandSeed)) >> 32
		mov     dwResult,edx
	}
	return(dwResult);
}
*/

class CRandom
{
private:
	volatile s32 holdrand;

public:
	CRandom() : holdrand(1) {};
	CRandom(s32 _seed) : holdrand(_seed) {};

	inline void seed(s32 val) { holdrand = val; }
	inline  s32 maxI() { return 32767; }

	__declspec(noinline)	s32 randI() { return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff); }
	inline s32 randI(s32 max) { VERIFY(max);  return randI() % max; }
	inline s32 randI(s32 min, s32 max) { return min + randI(max - min); }
	inline s32 randIs(s32 range) { return randI(-range, range); }
	inline s32 randIs(s32 range, s32 offs) { return offs + randIs(range); }

	inline float maxF() { return 32767.f; }
	inline float randF() { return float(randI()) / maxF(); }
	inline float randF(float max) { return randF()*max; }
	inline float randF(float min, float max) { return min + randF(max - min); }
	inline float randFs(float range) { return randF(-range, range); }
	inline float randFs(float range, float offs) { return offs + randFs(range); }
};

XRCORE_API extern CRandom	Random;

#endif