#pragma once
using namespace System;
#include "core\PostProcessAnimator.hpp"
namespace XRay
{
	namespace xmAPI
	{
		public enum class xmPostprocessParamType : int
		{
			Unknown = pp_params::pp_unknown,
			BaseColor = pp_params::pp_base_color,
			AddColor = pp_params::pp_add_color,
			GrayColor = pp_params::pp_gray_color,
			GrayValue = pp_params::pp_gray_value,
			Blur = pp_params::pp_blur,
			DualityH = pp_params::pp_dual_h,
			DualityV = pp_params::pp_dual_v,
			NoiseIntensity = pp_params::pp_noise_i,
			NoiseGrain = pp_params::pp_noise_g,
			NoiseFps = pp_params::pp_noise_f,
			ColorMappingInfluence = pp_params::pp_cm_influence,
			LastValue = pp_params::pp_last,
			ForceDword = pp_params::pp_force_dword,
		};

		public ref class xmPostprocessAnimator abstract
		{
		internal:
			::CPostprocessAnimator *impl;

		public:
			xmPostprocessAnimator() { impl = new CPostprocessAnimator(); }
			virtual				~xmPostprocessAnimator() { delete impl; }

			void				Clear() { impl->Clear(); }
			void				Load(LPCSTR name) { impl->Load(name); }
			LPCSTR				Name() { return impl->Name(); }
			virtual void		Stop(float speed) { return impl->Stop(speed); }
			void				SetDesiredFactor(float f, float sp) { impl->SetDesiredFactor(f, sp); }
			void				SetCurrentFactor(float f) { impl->SetCurrentFactor(f); }
			void				SetCyclic(bool b) { impl->SetCyclic(b); }
			float				GetLength() { return impl->GetLength(); }
			virtual	bool		Process(float dt, SPPInfo &PPInfo) override { return impl->Process(dt, PPInfo); };
			SPPInfo				EfParam() { return impl->EfParam(); }
			void				Create() { impl->Create(); }
			CPostProcessParam*  GetParam(pp_params param) { return impl->GetParam(param); }
			void				ResetParam(pp_params param) { impl->ResetParam(param); }
			void				Save(LPCSTR name) { impl->Save(name); }

		};

		public ref class xmPostprocessParam abstract
		{
		internal:
			::CPostProcessParam* impl;
		public:
			xmPostprocessParam() {};
			~xmPostprocessParam() {};
		};

		public ref class xmPostprocessColor abstract
		{
		internal:
			::CPostProcessColor* impl;
		public:
			xmPostprocessColor() {};
			~xmPostprocessColor() {};
		};

		public ref class xmPostprocessValue abstract
		{
		internal:
			::CPostProcessValue* impl;
		public:
			xmPostprocessValue() {};
			~xmPostprocessValue() {};
		};
	}
}