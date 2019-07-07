using System;
using System.Collections.Generic;

namespace xrSpectre
{
	public class RenderTarget : XRay.CRenderTarget
	{
		public RenderTarget() : base()
		{
			XRay.Log.Warning("RenderTarget Inited!");
		}
		
		public override void SecondaryCombine() 
		{
		//	XRay.Log.Warning("ACTOR UPDATE!");
		}
	}
}