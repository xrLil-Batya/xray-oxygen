#include "../xrCore/xrCore.h"
#include "mathTrig.h"

//
// Solve a*cos(theta) + b*sin(theta) = c
// Either one or two solutions. Return the answer in radians
//

int solve_trig1(float a, float b, float c, float theta[2])
{
	float temp = (a*a + b*b - c*c);

	if (temp < 0.0)
	{
		// temp is practically zero

		if (_abs(temp / (_abs(a*a) + _abs(b*b) + _abs(c*c))) < 1e-6)
		{
			// printf("Special case\n");
			theta[0] = (float)(2 * atan(-b / (-a - c)));
			return 1;
		}
		else
			return 0;
	}

	temp = (float)atan2((float)_sqrt(temp), (float)c);//.(float) c
	int num = (!iszero(temp)) ? 2 : 1;

	// Calculate answer in radians
	theta[0] = (float)atan2(b, a);
	if (num == 2)
	{
		theta[1] = theta[0] - temp;
		theta[0] += temp;

		//theta[0] = angle_normalize_signed(theta[0]);
		//theta[1] = angle_normalize_signed(theta[1]);
	}
	return num;
}

//
// Solve the system
//      a*cos(theta) - b*sin(theta) = c
//      a*sin(theta) + b*cos(theta) = d
//
// There is at most one solution. The answer is returned in radians
// 
float solve_trig2(float a, float b, float c, float d)
{
	return (float)atan2(a*d - b*c, a*c + b*d);
}

// 
// arccos routine that returns up to two solutions. 
//
int myacos(float x, float solns[2])
{
	if (_abs(x) > 1)
		return 0;

	solns[0] = angle_normalize_signed(acos(x));

	if (iszero(solns[0]))
		return 1;

	solns[1] = -solns[0];

	return 2;
}

// 
// arcsin routine that returns up to two solutions. 
//
int myasin(float x, float solns[2])
{
	if (_abs(x) > 1)
		return 0;

	solns[0] = (float)angle_normalize_signed(asin(x));

	if (iszero(solns[0]))
		return 1;

	if (solns[0] > 0)
		solns[1] = M_PI - solns[0];
	else
		solns[1] = -M_PI - solns[0];

	return 2;
}




