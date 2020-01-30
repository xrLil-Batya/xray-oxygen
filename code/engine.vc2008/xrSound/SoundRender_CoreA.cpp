#include "stdafx.h"
#pragma hdrstop
#include "SoundRender_CoreA.h"
#include "SoundRender_TargetA.h"

CSoundRender_CoreA*	SoundRenderA = 0;

CSoundRender_CoreA::CSoundRender_CoreA() :CSoundRender_Core()
{
	pDevice = 0;
	pDeviceList = 0;
	pContext = 0;
}

CSoundRender_CoreA::~CSoundRender_CoreA()
{
	alc_deinit();
}

void  CSoundRender_CoreA::_restart()
{
	inherited::_restart();
}

void* alXRayOxygen_MallocHook(size_t alignment, size_t size)
{
	return Memory.mem_alloc(size, alignment);
}

void* alXRayOxygen_CallocHook(size_t alignment, size_t size)
{
	void* memPtr = Memory.mem_alloc(size, alignment);
	ZeroMemory(memPtr, size);
	return memPtr;
}

void alXRayOxygen_FreeHook(void* ptr)
{
	Memory.mem_free(ptr);
}

size_t alXRayOxygen_GetPageSizeHook(void)
{
	static size_t psize = 0;
	if (psize == 0)
	{
		SYSTEM_INFO sysinfo;
		ZeroMemory(&sysinfo, sizeof(sysinfo));

		GetSystemInfo(&sysinfo);
		psize = sysinfo.dwPageSize;
	}
	return psize;
}

void CSoundRender_CoreA::_initialize(int stage)
{
	if (!stage)
	{
		alOxygenInitializeMemoryFunction(alXRayOxygen_MallocHook, alXRayOxygen_CallocHook, alXRayOxygen_FreeHook, alXRayOxygen_GetPageSizeHook);
		alc_init();

		pDeviceList = new ALDeviceList();

		if (0 == pDeviceList->GetNumDevices())
		{
			CHECK_OR_EXIT(0, "[OpenAL] Can't create sound device.");
			xr_delete(pDeviceList);
		}
		return;
	}

	pDeviceList->SelectBestDevice();
	R_ASSERT(snd_device_id >= 0 && snd_device_id < pDeviceList->GetNumDevices());
	const ALDeviceDesc& deviceDesc = pDeviceList->GetDeviceDesc(snd_device_id);
	// OpenAL device
    // alcOpenDevice can fail without any visible reason. Just try several times
    for (DWORD i = 0; i < 100; ++i)
    {
	    pDevice = alcOpenDevice(deviceDesc.name);
        if (pDevice != nullptr) break;
    }

	if (pDevice == nullptr)
	{
		CHECK_OR_EXIT(0, "[OpenAL] Failed to create device.");
		bPresent = false;
		return;
	}

	// Get the device specifier.
	const ALCchar* deviceSpecifier;
	deviceSpecifier = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);

	// Create context
	pContext = alcCreateContext(pDevice, nullptr);
	if (0 == pContext) {
		CHECK_OR_EXIT(0, "[OpenAL] Failed to create context.");
		bPresent = false;
		alcCloseDevice(pDevice); pDevice = 0;
		return;
	}

	// clear errors
	alGetError();
	alcGetError(pDevice);

	// Set active context
	AC_CHK(alcMakeContextCurrent(pContext));

	// initialize listener
	A_CHK(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));
	A_CHK(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));

	Fvector	orient[2] = { {0.f,0.f,1.f},{0.f,1.f,0.f} };

	A_CHK(alListenerfv(AL_ORIENTATION, &orient[0].x));
	A_CHK(alListenerf(AL_GAIN, 1.f));

	// Check for EFX extension
	if (deviceDesc.props.efx)
	{
		InitAlEFXAPI();
		bEFX = EFXTestSupport();
		//bEFX = false;
		Msg("[OpenAL] EFX: %s", bEFX ? "present" : "absent");
	}

	inherited::_initialize(stage);

	if (stage == 1)//first initialize
	{
		// Pre-create targets
		CSoundRender_Target* T = 0;
		for (u32 tit = 0; tit < u32(psSoundTargets); ++tit)
		{
			T = new CSoundRender_TargetA();
			if (T->_initialize())
			{
				if (bEFX)T->alAuxInit(slot);
				targets.push_back(T);
			}
			else
			{
				Msg("[OpenAL] ! SOUND: OpenAL: Max targets - %u", tit);
				T->_destroy();
				xr_delete(T);
				break;
			}
		}
	}
}

void CSoundRender_CoreA::set_master_volume(float f)
{
	if (bPresent)
		A_CHK(alListenerf(AL_GAIN, f));
}

void CSoundRender_CoreA::_clear()
{
	inherited::_clear();
	// remove targets
	CSoundRender_Target*	T = 0;
	for (u32 tit = 0; tit < targets.size(); tit++)
	{
		T = targets[tit];
		T->_destroy();
		xr_delete(T);
	}
	// Reset the current context to nullptr.
	alcMakeContextCurrent(nullptr);
	// Release the context and the device.
	alcDestroyContext(pContext);		pContext = 0;
	alcCloseDevice(pDevice);		pDevice = 0;
	xr_delete(pDeviceList);
}

void CSoundRender_CoreA::update_listener(const Fvector& P, const Fvector& D, const Fvector& N, float dt)
{
	inherited::update_listener(P, D, N, dt);

	Fvector velocity; velocity.set(0.0f, 0.0f, 0.0f);

	if (!Listener.position.similar(P))
	{
		Fvector diff = P;
		diff.sub(Listener.position);
		diff.mul(dt);
		velocity.set(diff);
		Listener.position.set(P);
		bListenerMoved = true;
	}
	Listener.orientation[0].set(D.x, D.y, -D.z);
	Listener.orientation[1].set(N.x, N.y, -N.z);

	A_CHK(alListener3f(AL_POSITION, Listener.position.x, Listener.position.y, -Listener.position.z));
	A_CHK(alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z));
	A_CHK(alListenerfv(AL_ORIENTATION, &Listener.orientation[0].x));
}