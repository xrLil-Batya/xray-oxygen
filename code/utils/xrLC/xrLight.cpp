#include "stdafx.h"
#include "build.h"

#include "../xrlc_light/xrdeflector.h"
#include "../xrLC_Light/xrLC_GlobalData.h"
#include "../xrLC_Light/xrLightVertex.h"
#include "../xrLC_Light/mu_model_light.h"
xrCriticalSection	task_CS;

xr_vector<int>		task_pool;

class CLMThread : public CThread
{
private:
    HASH			H;
    CDB::COLLIDER	DB;
    base_lighting	LightsSelected;
public:
    CLMThread(u32 ID) : CThread(ID, ProxyMsg)
    {
        // thMonitor= TRUE;
        thMessages = FALSE;
    }

    virtual void	Execute()
    {
        CDeflector* D = 0;

        while (true)
        {
			thProgress = 1.f - float(task_pool.size()) / float(lc_global_data()->g_deflectors().size());
            // Get task (Guard)
			{
				xrCriticalSectionGuard TaskGuard(task_CS);
				if (task_pool.empty())
				{
					//xrCriticalSectionGuard guard(task_CS);
					thProgress = 1.f - float(task_pool.size()) / float(lc_global_data()->g_deflectors().size());
					if (task_pool.empty()) return;

					D = lc_global_data()->g_deflectors()[task_pool.back()];
					task_pool.pop_back();
				}

				int DeflectorID = task_pool.back();
				D = lc_global_data()->g_deflectors()[DeflectorID];
				task_pool.pop_back();
			}
            // Perform operation
            D->Light(&DB, &LightsSelected, H);
        }
    }
};

void CBuild::LMapsLocal()
{
    mem_Compact();

    for (u32 dit = 0; dit<lc_global_data()->g_deflectors().size(); dit++)
        task_pool.push_back(dit);

    Logger.Status("Lighting...");
    CThreadManager	threads(ProxyStatus, ProxyProgress);

    u32	thNUM = 1;
    if (!g_build_options.b_optix_accel)
    {
        thNUM = CPU::Info.n_threads - 2;
    }
    //u32	thNUM = 5;
    CTimer	start_time;	start_time.Start();
    for (u32 L = 0; L<thNUM; L++)	threads.start(xr_new<CLMThread>(L));
    threads.wait(500);
    Logger.clMsg("%f seconds", start_time.GetElapsed_sec());
}

//routine enabled, when using new hardware light feature
void CBuild::LMapsRedux()
{
    mem_Compact();

    //new system not multithreaded... but using some handmade tricks to speedup coputation process
    Logger.Status("Lighting...");

    for (u32 dit = 0; dit < lc_global_data()->g_deflectors().size(); dit++)
        task_pool.push_back(dit);
}

void CBuild::LMaps()
{
	//****************************************** Lmaps
	Logger.Phase("LIGHT: LMaps...");
	LMapsLocal();
}

void CBuild::Light()
{
    //****************************************** Implicit
    {
        Logger.Phase("LIGHT: Implicit...");
        mem_Compact();
        ImplicitLighting();
    }

    LMaps();


    //****************************************** Vertex
    Logger.Phase("LIGHT: Vertex...");
    mem_Compact();

    LightVertex();
	WaitMuModelsLocalCalcLightening();
    //****************************************** Merge LMAPS
    {
        Logger.Phase("LIGHT: Merging lightmaps...");
        mem_Compact();

        xrPhase_MergeLM();
    }
}

void CBuild::LightVertex()
{
    ::LightVertex();
}