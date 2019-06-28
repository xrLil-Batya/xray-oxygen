#include "stdafx.h"
#include "CRenderTarget.h"

XRay::CRenderTarget::CRenderTarget() : NativeObject(::System::IntPtr(pRenderTarget))
{
	pNativeObject = pRenderTarget;
	AddVirtualMethod("SecondaryCombine");
}

XRay::CRenderTarget::CRenderTarget(::System::IntPtr inNativeObject) : NativeObject(inNativeObject)
{
	pNativeObject = (::CRenderTarget*)inNativeObject.ToPointer();
	AddVirtualMethod("SecondaryCombine");
}

void XRay::CRenderTarget::RenderScreenQuad(CFastResource^ ShaderResource, u32 Id)
{
	ref_rt outRT = RImplementation.o.dx10_msaa ? pNativeObject->rt_Generic : pNativeObject->rt_Color;

	pNativeObject->RenderScreenQuad(Device.dwWidth, Device.dwHeight, outRT, (*(ref_shader*)(void*)ShaderResource->Pointer)->E[Id]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), pNativeObject->rt_Generic_2->pTexture->surface_get());
}

XRay::CFastResource::CFastResource(::System::String^ XMLBLendName)
{
	NativeResource = new ref_shader();

	string64 NativeString = {};
	ConvertDotNetStringToAscii(XMLBLendName, NativeString);

	NativeResource->create(NativeString);
}
