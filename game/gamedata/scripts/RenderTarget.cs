using System;
using System.Collections.Generic;

namespace xrSpectre
{
	public class RenderTarget : XRay.CRenderTarget
	{
		public RenderTarget(IntPtr InNativeObject)
		: base(InNativeObject)
		{
			XRay.Log.Warning("RenderTarget Inited!");
		}
		
		public override void SecondaryCombine() 
		{
		//	XRay.Log.Warning("ACTOR UPDATE!");
		}
	}
}