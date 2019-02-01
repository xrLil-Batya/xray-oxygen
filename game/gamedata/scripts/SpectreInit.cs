using System;
using System.Collections.Generic;

namespace xrSpectre
{
    public class LoadTest : XRay.ModInstance
    {
        public override void OnLoad()
        {
			XRay.ClassRegistrator.Register(typeof(GamePersistence), (UInt64)XRay.EngineClassIDs.GamePersistence);
            XRay.Log.Info("! SpectreC#: Inited!");
	        XRay.Log.Info("! Spectrec#: Engine version atring: " + XRay.System.GetVersionString);
        }

        public override void OnShutdown()
        {
            XRay.Log.Info("! SpectreC#: Shutting down");
        }
    }
}
