#include "stdafx.h"
#include "../../xrEngine/xrlevel.h"

#include "xrThread.h"

#include "global_calculation_data.h"
#include "lightthread.h"
#include "xrLightDoNet.h"

#define NUM_THREADS		3

void	xrLight			()
{
	u32	range				= gl_data.slots_data.size_z();

	// Start threads, wait, continue --- perform all the work
	CThreadManager		Threads;
	CTimer				start_time;
	u32	stride			= range/NUM_THREADS;
	u32	last			= range-stride*	(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)	{
		CThread*	T		= xr_new<LightThread> (thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride));
		T->thMessages		= FALSE;
		T->thMonitor		= FALSE;
		Threads.start		(T);
	}
	Threads.wait			();
	Msg						("%d seconds elapsed.",(start_time.GetElapsed_ms())/1000);
}
#include "xrLC_GlobalData.h"
void xrCompileDO(bool net, bool rgb, bool sun)
{
	Phase		("Loading level...");
	gl_data.xrLoad	();

	Phase		("Lighting nodes...");

	if (!inlc_global_data())
		create_global_data();

	lc_global_data()->b_nosun_set(sun);
	lc_global_data()->b_skiplmap_set(rgb);

	if(net)
		lc_net::xrNetDOLight();
	else
		xrLight		();

	gl_data.slots_data.Free();
	
}
