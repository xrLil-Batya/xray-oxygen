#ifndef PH_WORLD_H
#define PH_WORLD_H
#include "Physics.h"
#include "phupdateobject.h"
#include "IPHWorld.h"
#include "physics_scripted.h"
#include "../xrEngine/pure.h"
// refs
struct	SGameMtlPair;

struct	SPHNetState;
class	CPHSynchronize;
typedef  xr_vector<std::pair<CPHSynchronize*, SPHNetState> > V_PH_WORLD_STATE;

class CPHMesh
{
	dGeomID Geom;
public:
	dGeomID GetGeom() { return Geom; }
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
};

#define PHWORLD_SOUND_CACHE_SIZE 8

////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class	CObjectSpace;
class	CObjectList;
class CPHWorld : public	pureFrame, public IPHWorld, public cphysics_scripted, public pureRender
{
	double m_start_time;
	u32 m_delay;
	u32 m_previous_delay;
	u32 m_reduce_delay;
	u32 m_update_delay_count;
	bool b_world_freezed;
	bool b_processing;
	bool b_exist;
	static const u32 update_delay = 1;

	CPHMesh Mesh;
	PH_OBJECT_STORAGE m_objects;
	PH_OBJECT_STORAGE m_freezed_objects;
	PH_OBJECT_STORAGE m_recently_disabled_objects;
	PH_UPDATE_OBJECT_STORAGE m_update_objects;
	PH_UPDATE_OBJECT_STORAGE m_freezed_update_objects;
	dGeomID m_motion_ray;

	IPHWorldUpdateCallbck *m_update_callback;
	CObjectSpace *m_object_space;
	CObjectList *m_level_objects;
	CRenderDeviceBase *m_device;

public:
	xr_vector<ISpatial*> r_spatial;

public:
	u64 m_steps_num;

private:
	u16 m_steps_short_num;

public:
	double m_frame_sum;
	dReal m_previous_frame_time;
	bool b_frame_mark;
	dReal m_frame_time;
	float m_update_time;
	u16 disable_count;
	float m_gravity;

private:
	ContactCallbackFun * m_default_contact_shotmark;
	ContactCallbackFun *m_default_character_contact_shotmark;
	PhysicsStepTimeCallback *physics_step_time_callback;
public:

	CPHWorld();
	CPHWorld(const CPHWorld& other) = delete;
	CPHWorld& operator=(const CPHWorld& other) = delete;
	virtual ~CPHWorld() {};

	inline	bool	Exist() { return b_exist; }
	void			Create(CObjectSpace * os, CObjectList *lo, CRenderDeviceBase* dv);
	void			SetGravity(float	g);
	inline  float	Gravity() { return m_gravity; }
	void			AddObject(CPHObject* object);
	void			AddUpdateObject(CPHUpdateObject* object);
	void			AddRecentlyDisabled(CPHObject* object);
	void			RemoveFromRecentlyDisabled(PH_OBJECT_I i);
	void			RemoveObject(PH_OBJECT_I i);
	void			RemoveUpdateObject(PH_UPDATE_OBJECT_I i);

	inline dGeomID GetMeshGeom() { return Mesh.GetGeom(); }
	inline	dGeomID GetMotionRayGeom() { return m_motion_ray; }

	void SetStep(float s);
	void Destroy();

	inline	float FrameTime(bool frame_mark) { return b_frame_mark == frame_mark ? m_frame_time : m_previous_frame_time; }

	void FrameStep(dReal step = 0.025f);
	void Step();
	void StepTouch();
	void CutVelocity(float l_limit, float a_limit);
	void GetState(V_PH_WORLD_STATE& state);
	void Freeze();
	void UnFreeze();
	void AddFreezedObject(CPHObject* obj);
	void RemoveFreezedObject(PH_OBJECT_I i);
	bool IsFreezed();

	inline	bool Processing() { return b_processing; }

	u32 CalcNumSteps(u32 dTime);
	u16 ObjectsNumber();
	u16 UpdateObjectsNumber();

	inline u16 StepsShortCnt() { return m_steps_short_num; }
	inline u64 &StepsNum() { return m_steps_num; }
	inline float FrameTime() { return	m_frame_time; }

	inline ContactCallbackFun *default_contact_shotmark() { return m_default_contact_shotmark; }
	inline ContactCallbackFun *default_character_contact_shotmark() { return m_default_character_contact_shotmark; }

	inline void set_default_contact_shotmark(ContactCallbackFun	*f) { m_default_contact_shotmark = f; }
	inline void set_default_character_contact_shotmark(ContactCallbackFun	*f) { m_default_character_contact_shotmark = f; }

	void NetRelcase(IPhysicsShellEx* s);

	inline CObjectSpace &ObjectSpace() { VERIFY(m_object_space); return *m_object_space; }
	inline CObjectList &LevelObjects() { VERIFY(m_level_objects); return *m_level_objects; }
	inline CRenderDeviceBase &Device() { VERIFY(m_device); return *m_device; }

	virtual void OnRender();
	virtual void __stdcall OnFrame();

private:
	void StepNumIterations(int num_it);
	inline iphysics_scripted &get_scripted() { return *this; }
	inline void set_step_time_callback(PhysicsStepTimeCallback* cb) { physics_step_time_callback = cb; }
	inline void set_update_callback(IPHWorldUpdateCallbck* cb) { VERIFY(cb); m_update_callback = cb; }
};

extern CPHWorld	*ph_world;

inline CPHWorld& inl_ph_world()
{
	return *ph_world;
}
#endif