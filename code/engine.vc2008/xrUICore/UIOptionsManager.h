#pragma once
#include "linker.h"

class CUIOptionsItem;
class UI_API CUIOptionsManager
{
	using group_name = shared_str;
	using items_list = xr_vector<CUIOptionsItem*>;
	using groups = xr_map<group_name, items_list>;
	using groups_it = groups::iterator;

public:	
		CUIOptionsManager					();

	void SaveBackupValues					(const shared_str& group);
	void SetCurrentValues					(const shared_str& group);
	void SaveValues							(const shared_str& group);
	void UndoGroup							(const shared_str& group);

	void OptionsPostAccept					();
	void DoVidRestart						();
	void DoSndRestart						();
	void DoSystemRestart					();

	bool NeedSystemRestart					()	{return 0 != (m_restart_flags&e_system_restart);}
	bool NeedVidRestart						()	{return 0 != (m_restart_flags&e_vid_restart);}
	void SendMessage2Group					(const shared_str& group, const char* message);

	void RegisterItem						(CUIOptionsItem* item, const shared_str& group);
	void UnRegisterItem						(CUIOptionsItem* item);

protected:	
	groups	m_groups;
	
	enum 
	{
		e_vid_restart    =(1<<0), 
		e_snd_restart    =(1<<1),
		e_system_restart =(1<<2)
	};
	u16		m_restart_flags;
};