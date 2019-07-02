#pragma once

#include "iinputreceiver.h"
#include "xr_object_list.h"
#include "../xrcdb/xr_area.h"

// refs
class ENGINE_API CCameraManager;
class ENGINE_API CCursor;
class ENGINE_API CCustomHUD;
class ENGINE_API ISpatial;
namespace Feel { class ENGINE_API Sound; }

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API	IGame_Level	:
	public DLL_Pure,
	public IInputReceiver,
	public pureRender,
	public pureFrame,
	public IEventReceiver
{
protected:
	// Network interface
	CObject*					pCurrentEntity;
	CObject*					pCurrentViewEntity;
   
	// Static sounds
	xr_vector<ref_sound>		Sounds_Random;
	u32							Sounds_Random_dwNextTime;
	BOOL						Sounds_Random_Enabled;
	CCameraManager*				m_pCameras;

	// temporary
	xr_vector<ISpatial*>		snd_ER;
public:
	CObjectList					Objects; 
	CObjectSpace				ObjectSpace;
	CCameraManager&				Cameras			()				{return *m_pCameras;};

	BOOL						bReady;

	CInifile*					pLevel;
public:	// deferred sound events
	struct	_esound_delegate
	{
		Feel::Sound*			dest;
		ref_sound_data_ptr		source;
		float					power;
	};
	xr_vector<_esound_delegate>	snd_Events;
public:
	// Main, global functions
	IGame_Level					();
	virtual ~IGame_Level		();

	virtual shared_str			name					() const = 0;
	virtual shared_str			name_translated			() const = 0;

	virtual BOOL				net_Start				(LPCSTR op_server, LPCSTR op_client)	= 0;
	virtual void				net_Stop				();
	virtual void				net_Update				()										= 0;

	virtual BOOL				Load					(u32 dwNum);
	virtual BOOL				Load_GameSpecific_Before()										{ return TRUE; };		// before object loading
	virtual BOOL				Load_GameSpecific_After	()										{ return TRUE; };		// after object loading
	virtual void				Load_GameSpecific_CFORM	(CDB::TRI* T, u32 count)				= 0;

	virtual void				OnFrame					();
	virtual void				OnRender				();

	virtual void				RenderBullets			() = 0;

	//возвращает время в милисекундах относительно начала игры
	virtual u64					GetStartGameTime		() { return 0; }
	virtual u64					GetGameTime				() { return 0; }

	// Main interface
	CObject*					CurrentEntity			() const							{ return pCurrentEntity; }
	CObject*					CurrentViewEntity		() const							{ return pCurrentViewEntity; }
	CObject*					CurrentControlEntity	() const							{ return nullptr; }

	void						SetEntity				(CObject* O);
	void						SetViewEntity			(CObject* O);
	
	void						SoundEvent_Register		(ref_sound_data_ptr S, float range);
	void						SoundEvent_Dispatch		();
	void                        SoundEvent_OnDestDestroy(Feel::Sound*);

	// Loader interface
	virtual void				SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor) = 0;
};

//-----------------------------------------------------------------------------------------------------------
extern ENGINE_API	IGame_Level*	g_pGameLevel;
