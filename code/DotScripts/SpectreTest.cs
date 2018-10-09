using System;
using System.Collections.Generic;

namespace xrSpectreTest
{
    public class LoadTest : XRay.ModInstance
    {
        public override void OnLoad()
        {
            XRay.Log.Info("Oxygen Test Mod initializing");
			XRay.ClassRegistrator.Register(typeof(OxygenExampleActor), (UInt64)XRay.GameClassIDs.Actor2);
        }
    }
}
