#include "stdafx.h"
#include "CPostprocessAnimator.h"
#include <xrCore\SPPInfo.h>
#include "PostProcessAnimator.h"

#pragma warning(push)
#pragma warning(disable : 4995) // ignore deprecation warnings
#include <msclr/marshal.h>
#pragma warning(pop)

using msclr::interop::marshal_context;

using namespace XRay;

PostProcessParamBase::PostProcessParamBase(::CPostProcessParam* impl)
{
	this->impl = impl;
}

PostProcessParamBase::~PostProcessParamBase()
{
	if (!dontDestroy)
	{
		auto p = impl;
		xr_delete(p);
	}
	impl = nullptr;
}

PostProcessParam::PostProcessParam(::CPostProcessValue* impl) : PostProcessParamBase(impl)
{
}

void PostProcessParam::Update(float dt)
{
	impl->update(dt);
}

void PostProcessParam::Load(File^ reader)
{
	::IReader& readerImpl = *(IReader*)reader->NativeReader.ToPointer();
	impl->load(readerImpl);
}
void PostProcessParam::Save(File^ writer)
{
	::IWriter& writerImpl = *(IWriter*)writer->NativeWritter.ToPointer();
	impl->save(writerImpl);
}

float PostProcessParam::Length::get()
{
	return impl->get_length();
}

int PostProcessParam::KeyCount::get()
{
	return (int)impl->get_keys_count();
}

void PostProcessParam::AddValue(float time, float value, int index)
{
	impl->add_value(time, value, index);
}

void PostProcessParam::DeleteValue(float time)
{
	impl->delete_value(time);
}

void PostProcessParam::UpdateValue(float time, float value, int index)
{
	impl->update_value(time, value, index);
}

void PostProcessParam::GetValue(float time, [Out] float % value, int index)
{
	float tmp;
	impl->get_value(time, tmp, index);
	value = tmp;
}

float PostProcessParam::GetKeyTime(int index)
{
	return impl->get_key_time(index);
}

void PostProcessParam::Reset()
{
	impl->clear_all_keys();
}

float PostProcessInfo::Blur::get()
{
	return impl->blur;
}

void PostProcessInfo::Blur::set(float value)
{
	impl->blur = value;
}

float PostProcessInfo::Gray::get()
{
	return impl->gray;
}

void PostProcessInfo::Gray::set(float value)
{
	impl->gray = value;
}

PostProcessInfo::Color PostProcessInfo::BaseColor::get()
{
	return reinterpret_cast<PostProcessInfo::Color&>(impl->color_base);
}

void PostProcessInfo::BaseColor::set(PostProcessInfo::Color value)
{
	impl->color_base = reinterpret_cast<::SPPInfo::SColor&>(value);
}

PostProcessInfo::Color PostProcessInfo::GrayColor::get()
{
	return reinterpret_cast<PostProcessInfo::Color&>(impl->color_gray);
}

void PostProcessInfo::GrayColor::set(PostProcessInfo::Color value)
{
	impl->color_gray = reinterpret_cast<::SPPInfo::SColor&>(value);
}

PostProcessInfo::Color PostProcessInfo::AddColor::get()
{
	return reinterpret_cast<PostProcessInfo::Color&>(impl->color_add);
}

void PostProcessInfo::AddColor::set(PostProcessInfo::Color value)
{
	impl->color_add = reinterpret_cast<::SPPInfo::SColor&>(value);
}

float PostProcessInfo::ColorMappingInfluence::get() { return impl->cm_influence; }
void PostProcessInfo::ColorMappingInfluence::set(float value) { impl->cm_influence = value; }
float PostProcessInfo::ColorMappingInterpolate::get() { return impl->cm_interpolate; }
void PostProcessInfo::ColorMappingInterpolate::set(float value) { impl->cm_interpolate = value; }
String ^ PostProcessInfo::ColorMappingGradient1::get() { return gcnew String(impl->cm_tex1.c_str()); }
void PostProcessInfo::ColorMappingGradient1::set(String ^ value)
{
	string512 TempString;
	ConvertDotNetStringToAscii(value, TempString);
	impl->cm_tex1 = TempString;
}
String ^ PostProcessInfo::ColorMappingGradient2::get() { return gcnew String(impl->cm_tex2.c_str()); }
void PostProcessInfo::ColorMappingGradient2::set(String ^ value)
{
	string512 TempString;
	ConvertDotNetStringToAscii(value, TempString);
	impl->cm_tex2 = TempString;
}
PostProcessInfo::PostProcessInfo(::SPPInfo* impl) { this->impl = impl; }
PostProcessInfo::PostProcessInfo(::SPPInfo* impl, bool dontDestroy) : PostProcessInfo(impl)
{
	this->dontDestroy = dontDestroy;
}
PostProcessInfo::~PostProcessInfo()
{
	if (!dontDestroy)
	{
		auto p = impl;
		xr_delete(p);
	}
	impl = nullptr;
}
PostProcessInfo % PostProcessInfo::Add(const PostProcessInfo % ppi)
{
	impl->add(*ppi.impl);
	return *this;
}
PostProcessInfo % PostProcessInfo::Substract(const PostProcessInfo % ppi)
{
	impl->sub(*ppi.impl);
	return *this;
}
void PostProcessInfo::Normalize() { impl->normalize(); }
PostProcessInfo % PostProcessInfo::Interpolate(const PostProcessInfo % def, const PostProcessInfo % to, float factor)
{
	impl->lerp(*def.impl, *to.impl, factor);
	return *this;
}
void PostProcessInfo::Validate(String ^ str)
{
	string512 TempString;
	ConvertDotNetStringToAscii(str, TempString);
	impl->validate(TempString);
}

PostProcessParamProxy::PostProcessParamProxy(::CPostProcessParam* impl) : PostProcessParamBase(impl)
{
	dontDestroy = true;
}
void PostProcessParamProxy::Update(float dt)
{
	impl->update(dt);
}

void PostProcessParamProxy::Load(File^ reader)
{
	impl->load(*(IReader*)reader->NativeReader.ToPointer());
}

void PostProcessParamProxy::Save(File^ writer)
{
	impl->save(*(IWriter*)writer->NativeWritter.ToPointer());
}

float PostProcessParamProxy::Length::get()
{
	return impl->get_length();
}

int PostProcessParamProxy::KeyCount::get()
{
	return (int)impl->get_keys_count();
}

void PostProcessParamProxy::AddValue(float time, float value, int index)
{
	impl->add_value(time, value, index);
}

void PostProcessParamProxy::DeleteValue(float time)
{
	impl->delete_value(time);
}

void PostProcessParamProxy::UpdateValue(float time, float value, int index)
{
	impl->update_value(time, value, index);
}

void PostProcessParamProxy::GetValue(float time, [Out] float % value, int index)
{
	float tmp;
	impl->get_value(time, tmp, index);
	value = tmp;
}

float PostProcessParamProxy::GetKeyTime(int index)
{
	return impl->get_key_time(index);
}

void PostProcessParamProxy::Reset()
{
	impl->clear_all_keys();
}

XRay::BasicPostProcessAnimator::BasicPostProcessAnimator() : impl(new ::BasicPostProcessAnimator())
{
}

XRay::BasicPostProcessAnimator::BasicPostProcessAnimator(int id, bool cyclic)
	: impl(new ::BasicPostProcessAnimator(id, cyclic))
{
}

XRay::BasicPostProcessAnimator::~BasicPostProcessAnimator()
{
	auto p = impl;
	xr_delete(p);
	impl = nullptr;
}
void XRay::BasicPostProcessAnimator::Clear()
{
	impl->Clear();
}

void XRay::BasicPostProcessAnimator::Load(String ^ name, bool internalFs)
{
	string512 TempString;
	ConvertDotNetStringToAscii(name, TempString);

	impl->Load(TempString, internalFs);
}

String ^ XRay::BasicPostProcessAnimator::Name::get()
{
	return gcnew String(impl->Name());
}

void XRay::BasicPostProcessAnimator::Stop(float speed)
{
	impl->Stop(speed);
}
void XRay::BasicPostProcessAnimator::SetDesiredFactor(float f, float sp)
{
	impl->SetDesiredFactor(f, sp);
}

void XRay::BasicPostProcessAnimator::SetCurrentFactor(float f)
{
	impl->SetCurrentFactor(f);
}

void XRay::BasicPostProcessAnimator::SetCyclic(bool b)
{
	impl->SetCyclic(b);
}

float XRay::BasicPostProcessAnimator::Length::get()
{
	return impl->GetLength();
}

PostProcessInfo ^ XRay::BasicPostProcessAnimator::PPInfo::get()
{
	return gcnew PostProcessInfo(&impl->PPinfo(), true);
}

bool XRay::BasicPostProcessAnimator::Process(float dt, PostProcessInfo ^ PPInfo)
{
	return !!impl->Process(dt, *PPInfo->impl);
}

void XRay::BasicPostProcessAnimator::Create()
{
	impl->Create();
}

PostProcessParamBase ^ XRay::BasicPostProcessAnimator::GetParam(PostProcessParamType param)
{
	return gcnew PostProcessParamProxy(impl->GetParam((_pp_params)param));
}

void XRay::BasicPostProcessAnimator::ResetParam(PostProcessParamType param)
{
	impl->ResetParam((_pp_params)param);
}

void XRay::BasicPostProcessAnimator::Save(String ^ name)
{
	string512 TempString;
	ConvertDotNetStringToAscii(name, TempString);
	impl->Save(TempString);
}