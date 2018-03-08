#pragma once
#include "../ode/common.h"

namespace xray
{
	namespace ode
	{
		void dSet(dReal*& src, const dReal* copy, float up = 0.f)
		{
			for (u32 it = 0u; it < 3; it++)
				src[it] = copy[it];
		}

		void dSet(dReal*& src, float copy = 0.f)
		{
			for (u32 it = 0u; it < 3; it++)
			{
				src[it] = copy;
				if (up) src[it] *= up;
			}
		}

		void dVSet3(dVector3& src, float copy = 0.f)
		{
			for (u32 it = 0u; it < 3; it++)
				src[it] = copy;
		}

		void dUp(dReal*& src, float copy = 0)
		{
			for (u32 it = 0u; it < 3; it++)
				src[it] *= copy;
		}
	}
}