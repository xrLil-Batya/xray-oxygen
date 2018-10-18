#include "Pch.hpp"
#include "fs\FS.hpp"
#include "IPostProcessAnimator.hpp"
#include "xrCore/xrCore.h"

#pragma warning(push)
#pragma warning(disable : 4995) // ignore deprecation warnings
#include <msclr/marshal.h>
#pragma warning(pop)

using msclr::interop::marshal_context;
using XRay::ManagedApi::Core::PostProcessParamBase;
using XRay::ManagedApi::Core::PostProcessParam;
using XRay::ManagedApi::Core::PostProcessInfo;
using XRay::ManagedApi::Core::PostProcessParamProxy;
using XRay::ManagedApi::Core::ReaderBase;
using XRay::ManagedApi::Core::WriterBase;

PostProcessParamBase::PostProcessParamBase(::CPostProcessParam* impl) { this->impl = impl; }
PostProcessParamBase::~PostProcessParamBase()
{
	if (!dontDestroy)
	{
		auto p = impl;
		xr_delete(p);
	}
	impl = nullptr;
}

PostProcessParam::PostProcessParam(::CPostProcessValue* impl) : PostProcessParamBase(impl) {}
void PostProcessParam::Update(float dt) { impl->update(dt); }

void PostProcessParam::Load(ReaderBase ^ reader)
{
	::IReader& readerImpl = *reader->impl;
	impl->load(readerImpl);
}
void PostProcessParam::Save(WriterBase ^ writer)
{
	::IWriter& writerImpl = *writer->impl;
	impl->save(writerImpl);
}
float PostProcessParam::Length::get() { return impl->get_length(); }
int PostProcessParam::KeyCount::get() { return (int)impl->get_keys_count(); }
void PostProcessParam::AddValue(float time, float value, int index) { impl->add_value(time, value, index); }
void PostProcessParam::DeleteValue(float time) { impl->delete_value(time); }

void PostProcessParam::UpdateValue(float time, float value, int index)
{
	impl->update_value(time, value / 255, index);
}

void XRay::ManagedApi::Core::PostProcessParam::GetValue(float time, [Out] float % value, int index)
{
	float tmp;
	impl->get_value(time, tmp, index);
	value = tmp;
}
float PostProcessParam::GetKeyTime(int index) { return impl->get_key_time(index); }
void PostProcessParam::Reset() { impl->clear_all_keys(); }
float PostProcessInfo::Blur::get() { return impl->blur; }
void PostProcessInfo::Blur::set(float value) { impl->blur = value; }
float PostProcessInfo::Gray::get() { return impl->gray; }
void PostProcessInfo::Gray::set(float value) { impl->gray = value; }
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
System::String^ PostProcessInfo::ColorMappingGradient1::get() { return gcnew String(impl->cm_tex1.c_str()); }
void PostProcessInfo::ColorMappingGradient1::set(System::String ^ value)
{
	marshal_context context;
	auto tmpStr = context.marshal_as<const char*>(value);
	impl->cm_tex1 = tmpStr;
}
System::String^ PostProcessInfo::ColorMappingGradient2::get() { return gcnew String(impl->cm_tex2.c_str()); }
void PostProcessInfo::ColorMappingGradient2::set(System::String ^ value)
{
	marshal_context context;
	auto tmpStr = context.marshal_as<const char*>(value);
	impl->cm_tex2 = tmpStr;
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
void PostProcessInfo::Validate(System::String ^ str)
{
	marshal_context context;
	auto tmpStr = context.marshal_as<const char*>(str);
	impl->validate(tmpStr);
}

PostProcessParamProxy::PostProcessParamProxy(::CPostProcessParam* impl) : PostProcessParamBase(impl)
{
	dontDestroy = true;
}
void PostProcessParamProxy::Update(float dt) { impl->update(dt); }
void PostProcessParamProxy::Load(ReaderBase ^ reader) { impl->load(*reader->impl); }
void PostProcessParamProxy::Save(WriterBase ^ writer) { impl->save(*writer->impl); }
float PostProcessParamProxy::Length::get() { return impl->get_length(); }
int PostProcessParamProxy::KeyCount::get() { return impl->get_keys_count(); }
void PostProcessParamProxy::AddValue(float time, float value, int index) { impl->add_value(time, value, index); }
void PostProcessParamProxy::DeleteValue(float time) { impl->delete_value(time); }
void PostProcessParamProxy::UpdateValue(float time, float value, int index) 
{
	impl->update_value(time, value / 255, index);
}
void XRay::ManagedApi::Core::PostProcessParamProxy::GetValue(float time, [Out] float % value, int index)
{
	float tmp;
	impl->get_value(time, tmp, index);
	value = tmp;
}
float PostProcessParamProxy::GetKeyTime(int index) { return impl->get_key_time(index); }
void PostProcessParamProxy::Reset() { impl->clear_all_keys(); }
XRay::ManagedApi::Core::BasicPostProcessAnimator::BasicPostProcessAnimator() : impl(new ::BasicPostProcessAnimator()) {}
XRay::ManagedApi::Core::BasicPostProcessAnimator::BasicPostProcessAnimator(int id, bool cyclic)
	: impl(new ::BasicPostProcessAnimator(id, cyclic))
{
}

XRay::ManagedApi::Core::BasicPostProcessAnimator::~BasicPostProcessAnimator()
{
	auto p = impl;
	xr_delete(p);
	impl = nullptr;
}
void XRay::ManagedApi::Core::BasicPostProcessAnimator::Clear() { impl->Clear(); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::Load(System::String ^ name, bool internalFs)
{
	marshal_context context;
	// 	std::string new_name = "";
	// 	char slash_name = '\\';
	const char* tmpName = context.marshal_as<const char*>(name);
	// 	for (size_t it = 0; it < strlen(tmpName); it++)
	// 	{
	// 		new_name += tmpName[it];
	// 		if (tmpName[it] == slash_name)
	// 			new_name += slash_name;
	// 	}

	impl->Load(tmpName, internalFs);
}
System::String ^ XRay::ManagedApi::Core::BasicPostProcessAnimator::Name::get() { return gcnew String(impl->Name()); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::Stop(float speed) { impl->Stop(speed); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::SetDesiredFactor(float f, float sp) { impl->SetDesiredFactor(f, sp); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::SetCurrentFactor(float f) { impl->SetCurrentFactor(f); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::SetCyclic(bool b) { impl->SetCyclic(b); }
float XRay::ManagedApi::Core::BasicPostProcessAnimator::Length::get() { return impl->GetLength(); }
PostProcessInfo ^ XRay::ManagedApi::Core::BasicPostProcessAnimator::PPInfo::get() { return gcnew PostProcessInfo(&impl->PPinfo(), true); }
bool XRay::ManagedApi::Core::BasicPostProcessAnimator::Process(float dt, PostProcessInfo ^ PPInfo)
{
	return !!impl->Process(dt, *PPInfo->impl);
}
void XRay::ManagedApi::Core::BasicPostProcessAnimator::Create() { impl->Create(); }
PostProcessParamBase ^ XRay::ManagedApi::Core::BasicPostProcessAnimator::GetParam(PostProcessParamType param)
{
	return gcnew PostProcessParamProxy(impl->GetParam((pp_params)param));
}
void XRay::ManagedApi::Core::BasicPostProcessAnimator::ResetParam(PostProcessParamType param) { impl->ResetParam((pp_params)param); }
void XRay::ManagedApi::Core::BasicPostProcessAnimator::Save(System::String ^ name)
{
	marshal_context context;
	auto tmpName = context.marshal_as<const char*>(name);
	impl->Save(tmpName);
}