#include "stdafx.h"
#include "../../xrEngine/xrlevel.h"

#include "global_calculation_data.h"
#include "lightthread.h"

void xrLight()
{
	u32	range = gl_data.slots_data.size_z();

	// Start threads, wait, continue --- perform all the work
	const u32 thrds_count = CPU::Info.n_threads;
	CThreadManager Threads(ProxyStatus, ProxyProgress);
	CTimer start_time;
	u32	stride = range / thrds_count;
	u32	last = range - stride*	(thrds_count - 1);

	for (u32 thID = 0; thID < thrds_count; thID++)
	{
		CThread* T = xr_new<LightThread>(thID, thID*stride, thID*stride + ((thID == (thrds_count - 1)) ? last : stride));
		T->thMessages = FALSE;
		T->thMonitor = FALSE;
		Threads.start(T);
	}

	Threads.wait();
	Msg("%d seconds elapsed.", (start_time.GetElapsed_ms()) / 1000);
}

#include "xrLC_GlobalData.h"
void xrCompileDO(bool rgb, bool sun)
{
	Logger.Phase("Loading level...");
	gl_data.xrLoad();
	Logger.Phase("Lighting nodes...");
	{
		if (!inlc_global_data())
			create_global_data();

		lc_global_data()->b_nosun_set(sun);
		lc_global_data()->b_skiplmap_set(rgb);

		xrLight();
	}
	destroy_global_data();
	gl_data.slots_data.Free();
}
