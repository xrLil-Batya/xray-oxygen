////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_data_storage.h
//	Created 	: 13.10.2005
//  Modified 	: 13.10.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation data storage
////////////////////////////////////////////////////////////////////////////
#pragma once

class CStalkerAnimationData;
class IKinematicsAnimated;

class CStalkerAnimationDataStorage 
{
public:
	typedef std::pair<IKinematicsAnimated*,CStalkerAnimationData*>		OBJECT;
	typedef xr_vector<OBJECT>											OBJECTS;

private:
	OBJECTS								m_objects;

public:
	virtual								~CStalkerAnimationDataStorage	();
			const CStalkerAnimationData	*object							(IKinematicsAnimated *skeleton_animated);
			void						clear							();
};

extern CStalkerAnimationDataStorage *g_stalker_animation_data_storage;

IC	CStalkerAnimationDataStorage& stalker_animation_data_storage()
{
	if (g_stalker_animation_data_storage)
		return							(*g_stalker_animation_data_storage);

	g_stalker_animation_data_storage =  new CStalkerAnimationDataStorage();
	return								(*g_stalker_animation_data_storage);
}
