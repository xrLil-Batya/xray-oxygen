using System;
using System.Collections.Generic;

namespace xrSpectreTest
{
	public class OxygenExampleActor : XRay.Actor
	{
		public OxygenExampleActor(IntPtr InNativeObject)
		: base(InNativeObject)
		{}
		
		public override void shedule_update(int updateInverval) 
		{
			XRay.Log.Warning("ACTOR UPDATE!");
		}
	}
}