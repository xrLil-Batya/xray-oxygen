#include "stdafx.h"
#include "ObjectPool.h"
#include "ClassRegistrator.h"

using namespace System::Linq::Expressions;
using namespace System::Reflection;

UInt32 XRay::ObjectPool::CreateObject(IntPtr pDllPure)
{
	ConditionalInitialize();
	DLL_Pure* DllPure = reinterpret_cast<DLL_Pure*>(pDllPure.ToPointer());
	Type^ ClassType = ClassRegistrator::GetTypeForClassId(DllPure->CLS_ID);

	if (ClassType == nullptr)
	{
		string16 temp; CLSID2TEXT(DllPure->CLS_ID, temp);
		string256 Msg;
		strconcat(sizeof(Msg), Msg, "There is no class type: ", temp);
		XRay::Log::Warning(gcnew String(Msg));
		return 0;
	}

	// Get compiled lambda activator for that type
	ObjectActivator^ Activator = nullptr;
	if (!ConstructorCache->TryGetValue(ClassType, Activator))
	{
		Activator = CreateFastObjectActivator(ClassType);
		ConstructorCache->Add(ClassType, Activator);
	}

	IntPtr BoxedPtr(DllPure);

	List< XRay::NativeObject^>^ ObjectList = nullptr;
	if (!GlobalRegistry->TryGetValue(BoxedPtr, ObjectList))
	{
		ObjectList = gcnew List<XRay::NativeObject ^>();
		GlobalRegistry->Add(BoxedPtr, ObjectList);
	}

	// Call activator to create a newer object
	XRay::NativeObject^ CreatedObject = Activator(BoxedPtr);
	ObjectList->Add(CreatedObject);

	IndexRegistry->Add(++IndexCounter, CreatedObject);

	return IndexCounter;
}

void XRay::ObjectPool::DestroyObject(IntPtr pDllPure)
{
	List< XRay::NativeObject^>^ ObjectList = nullptr;
	DLL_Pure* DllPure = reinterpret_cast<DLL_Pure*>(pDllPure.ToPointer());
	if (GlobalRegistry->TryGetValue(pDllPure, ObjectList))
	{
		for each (XRay::NativeObject^ NativeObj in ObjectList)
		{
			NativeObj->OnDestroyed();
		}
		ObjectList->Clear();
		GlobalRegistry->Remove(pDllPure);
		IndexRegistry->Remove(DllPure->SpectreObjectId);
	}
}


XRay::NativeObject^ XRay::ObjectPool::GetObjectByHandle(UInt32 Handle)
{
	XRay::NativeObject^ NatObject = nullptr;
	IndexRegistry->TryGetValue(Handle, NatObject);
	return NatObject;
}

void XRay::ObjectPool::ConditionalInitialize()
{
	if (GlobalRegistry == nullptr)
	{
		GlobalRegistry = gcnew Dictionary<IntPtr, List<XRay::NativeObject ^> ^>();
		ConstructorCache = gcnew Dictionary < Type^, ObjectActivator^ >();
		IndexRegistry = gcnew Dictionary<UInt32, XRay::NativeObject ^>();
		IndexCounter = 0;
	}
}

XRay::ObjectPool::ObjectActivator^ XRay::ObjectPool::CreateFastObjectActivator(Type^ ClassType)
{
	array<Type^>^ constructorsParams = gcnew array<Type ^>(1);
	constructorsParams[0] = IntPtr::typeid;

	ConstructorInfo^ NativeConstructor = ClassType->GetConstructor(BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::Instance, nullptr, constructorsParams, nullptr);
	ParameterExpression^ InNativeObjectParam = Expression::Parameter(IntPtr::typeid);

	NewExpression^ ConstructorExpression = Expression::New(NativeConstructor, InNativeObjectParam);
	LambdaExpression^ lambda = Expression::Lambda(ObjectActivator::typeid, ConstructorExpression, InNativeObjectParam);

	return (ObjectActivator^)lambda->Compile();
}
