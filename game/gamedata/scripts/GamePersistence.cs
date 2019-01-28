using System;
using System.Collections.Generic;

namespace xrSpectre
{
	public class GamePersistence : XRay.CGamePersistence
	{
		public GamePersistence(IntPtr InNativeObject)
		: base(InNativeObject)
		{
			
		}
		
		public override void shedule_update(UInt32 updateInverval) 
		{
			XRay.Log.Warning("ACTOR UPDATE!");
		}
	}
}