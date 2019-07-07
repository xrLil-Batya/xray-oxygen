#pragma once
namespace XRay
{
	public ref class CFastResource
	{
		ref_shader* NativeResource;
	public:
		CFastResource(::System::String^ XMLBLendName);

		property ::System::IntPtr Pointer
		{
			::System::IntPtr get()
			{
				return (::System::IntPtr)NativeResource;
			}
		};
	};

	public ref class CRenderTarget abstract: NativeObject
	{
		::CRenderTarget* pNativeObject;
	public:
		CRenderTarget();
		CRenderTarget(::System::IntPtr pNativeObject);

		void			RenderScreenQuad(CFastResource^ ShaderResource, u32 Id);
		virtual void	SecondaryCombine() = 0;
	};
}
