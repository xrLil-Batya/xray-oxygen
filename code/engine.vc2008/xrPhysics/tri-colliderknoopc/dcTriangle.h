#pragma		once
struct Triangle
{
	dVector3 side0;
	dVector3 side1;
	dVector3 norm;
	dReal dist;
	dReal pos;
	dReal depth;
	CDB::TRI* T;
	Triangle()
	{
		T = NULL;
#ifdef DEBUG
		depth = -dInfinity;
		dist = -dInfinity;
#endif
	}
	};