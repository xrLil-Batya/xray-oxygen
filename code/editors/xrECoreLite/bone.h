//----------------------------------------------------
#pragma once
#ifdef _LW_PLUGIN
#include <lwtypes.h>
#endif
//*** Shared Bone Data ****************************************************************************
class CBoneData;

// t-defs
typedef xr_vector<CBoneData*> vecBones;
typedef vecBones::iterator vecBonesIt;

const u16 BI_NONE = u16(-1);

#define OGF_IKDATA_VERSION 0x0001

#pragma pack( push,1 )
enum EJointType
{
	jtRigid,
	jtCloth,
	jtJoint,
	jtWheel,
	jtNone,
	jtSlider,
	jtForceU32 = u32(-1)
};

struct ECORE_API SJointLimit
{
	Fvector2 limit;
	float spring_factor;
	float damping_factor;

	void Reset()
	{
		limit.set(0.f, 0.f);
		spring_factor = 1.f;
		damping_factor = 1.f;
	}

	SJointLimit()
	{
		Reset();
	}
};

struct ECORE_API SBoneShape
{
	enum EShapeType
	{
		stNone,
		stBox,
		stSphere,
		stCylinder,
		stForceU32 = u16(-1)
	};

	enum EShapeFlags
	{
		sfNoPickable = (1 << 0), 	// use only in RayPick
		sfRemoveAfterBreak = (1 << 1),
		sfNoPhysics = (1 << 2),
	};

	u16 type;		// 2
	Flags16	flags;		// 2
	Fobb box;      	// 15*4
	Fsphere sphere;		// 4*4
	Fcylinder cylinder;	// 8*4

	void			Reset()
	{
		flags.zero();
		type = stNone;
		box.invalidate();
		sphere.P.set(0.f, 0.f, 0.f); sphere.R = 0.f;
		cylinder.invalidate();
	}

	SBoneShape()
	{
		Reset();
	}

	bool Valid()
	{
		switch (type)
		{
		case stBox:
			return !fis_zero(box.m_halfsize.x) && !fis_zero(box.m_halfsize.y) && !fis_zero(box.m_halfsize.z);
		case stSphere:
			return !fis_zero(sphere.R);
		case stCylinder:
			return !fis_zero(cylinder.m_height) && !fis_zero(cylinder.m_radius) && !fis_zero(cylinder.m_direction.square_magnitude());
		};

		return true;
	}
};

struct ECORE_API SJointIKData
{
	// IK
	EJointType type;
	SJointLimit limits[3];// by [axis XYZ on joint] and[Z-wheel,X-steer on wheel]
	float spring_factor;
	float damping_factor;

	enum {
		flBreakable = (1 << 0),
	};

	Flags32 ik_flags;
	float break_force;	// [0..+INF]
	float break_torque;	// [0..+INF]

	float friction;

	void Reset()
	{
		limits[0].Reset();
		limits[1].Reset();
		limits[2].Reset();
		type = EJointType::jtRigid;
		spring_factor = 1.f;
		damping_factor = 1.f;
		ik_flags.zero();
		break_force = 0.f;
		break_torque = 0.f;
	}

	SJointIKData()
	{
		Reset();
	}

	void clamp_by_limits(Fvector& dest_xyz);
	void Export(IWriter& F)
	{
		F.w_u32(type);

		for (int k = 0; k < 3; ++k)
		{
			// Kostya Slipchenko say:
			// направление вращения в ОДЕ отличается от направления вращение в X-Ray
			// поэтому меняем знак у лимитов
			F.w_float(std::min(-limits[k].limit.x, -limits[k].limit.y)); // min (swap special for ODE)
			F.w_float(std::max(-limits[k].limit.x, -limits[k].limit.y)); // max (swap special for ODE)
			F.w_float(limits[k].spring_factor);
			F.w_float(limits[k].damping_factor);
		}

		F.w_float(spring_factor);
		F.w_float(damping_factor);

		F.w_u32(ik_flags.get());
		F.w_float(break_force);
		F.w_float(break_torque);

		F.w_float(friction);
	}
	bool Import(IReader& F, u16 vers)
	{
		type = (EJointType)F.r_u32();
		F.r(limits, sizeof(SJointLimit) * 3);
		spring_factor = F.r_float();
		damping_factor = F.r_float();
		ik_flags.flags = F.r_u32();
		break_force = F.r_float();
		break_torque = F.r_float();

		if (vers > 0)
			friction = F.r_float();

		return true;
	}
};
#pragma pack( pop )

// refs
class CBone;
using BoneVec = xr_vector<CBone*>;
using BoneIt = BoneVec::iterator;

class ECORE_API CBone
{
	shared_str name;
	shared_str parent_name;
	shared_str wmap;
	Fvector rest_offset;
	Fvector rest_rotate;    // XYZ format (Game format)
	float rest_length;

	Fvector mot_offset;
	Fvector mot_rotate;		// XYZ format (Game format)
	float mot_length;

	Fmatrix mot_transform;

	Fmatrix rest_transform;
	Fmatrix rest_i_transform;

	Fmatrix	last_transform;

	Fmatrix render_transform;
public:
	int SelfID;
	CBone *parent;
	BoneVec children;
public:
	// editor part
	Flags8 flags;

	enum {
		flSelected = (1 << 0),
	};

	SJointIKData IK_data;
	shared_str game_mtl;
	SBoneShape shape;

	float mass;
	Fvector center_of_mass;
public:
	CBone();
	virtual ~CBone();

	inline void SetName(const char* p) { name = p; xr_strlwr(name); }
	inline void SetParentName(const char* p) { parent_name = p; xr_strlwr(parent_name); }
	inline void SetWMap(const char* p) { wmap = p; }
	inline void SetRestParams(float length, const Fvector& offset, const Fvector& rotate) { rest_offset.set(offset); rest_rotate.set(rotate); rest_length = length; };
	inline shared_str Name() { return name; }
	inline shared_str ParentName() { return parent_name; }
	inline shared_str WMap() { return wmap; }
	inline CBone* Parent() { return parent; }
	inline BOOL	IsRoot() { return (parent == 0); }
	inline Fmatrix& _RTransform() { return rest_transform; }
	inline Fmatrix& _RITransform() { return rest_i_transform; }
	inline Fmatrix& _MTransform() { return mot_transform; }
	inline Fmatrix& _LTransform() { return last_transform; }
	inline Fmatrix& _RenderTransform() { return render_transform; }
	inline Fvector& _RestOffset() { return rest_offset; }
	inline Fvector& _RestRotate() { return rest_rotate; }

	// transformation
	inline const  Fvector& _Offset() { return mot_offset; }
	inline const Fvector& _Rotate() { return mot_rotate; }
	inline float _Length() { return mot_length; }
	inline void _Update(const Fvector& T, const Fvector& R) { mot_offset.set(T); mot_rotate.set(R); mot_length = rest_length; }
	inline void Reset() { mot_offset.set(rest_offset); mot_rotate.set(rest_rotate); mot_length = rest_length; }

	// IO
	void Save(IWriter& F);
	void Load_0(IReader& F);
	void Load_1(IReader& F);

#ifdef _LW_EXPORT
	void ParseBone(LWItemID bone);
#endif

	void SaveData(IWriter& F);
	void LoadData(IReader& F);
	void ResetData();
	void CopyData(CBone* bone);

#ifdef _EDITOR
	void ShapeScale(const Fvector& amount);
	void ShapeRotate(const Fvector& amount);
	void ShapeMove(const Fvector& amount);
	void BindRotate(const Fvector& amount);
	void BindMove(const Fvector& amount);
	void BoneMove(const Fvector& amount);
	void BoneRotate(const Fvector& axis, float angle);

	bool Pick(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& parent);

	inline void Select(BOOL flag) { flags.set(flSelected, flag); }
	inline bool Selected() { return flags.is(flSelected); }

	void ClampByLimits();
#endif
	bool ExportOGF(IWriter& F);
};

#pragma pack(push,8)
class ENGINE_API		CBoneInstance
{
public:
	typedef void  __stdcall BoneCallbackFunction(CBoneInstance* P);
	typedef BoneCallbackFunction* BoneCallback;

	// data
	Fmatrix mTransform;							// final x-form matrix (local to model)
	Fmatrix mRenderTransform;					// final x-form matrix (model_base -> bone -> model)
	float param[4];	//
					//
					// methods
private:
	BoneCallback Callback;
	void* Callback_Param;
	BOOL Callback_overwrite;					// performance hint - don't calc anims
	u32	Callback_type;

public:
	inline	BoneCallback __stdcall callback() { return  Callback; }
	inline	void* __stdcall callback_param() { return Callback_Param; }
	inline	BOOL __stdcall callback_overwrite() { return Callback_overwrite; }					// performance hint - don't calc anims
	inline	u32 __stdcall callback_type() { return Callback_type; }
	inline void	__stdcall construct();
	inline void __stdcall set_callback_overwrite(BOOL v) { Callback_overwrite = v; }
	inline u32	mem_usage() { return sizeof(*this); }

public:

	void __stdcall set_callback(u32 Type, BoneCallback C, void* Param, BOOL overwrite = FALSE)
	{
		Callback = C;
		Callback_Param = Param;
		Callback_overwrite = overwrite;
		Callback_type = Type;
	}

	void __stdcall reset_callback()
	{
		Callback = 0;
		Callback_Param = 0;
		Callback_overwrite = 0;
		Callback_type = 0;
	}

	void set_param(u32 idx, float data);
	float get_param(u32 idx);
};
#pragma pack(pop)

class ENGINE_API CBoneData
{
protected:
	u16	SelfID;
	u16	ParentID;
public:
	shared_str name;

	Fobb obb;

	Fmatrix	bind_transform;
	Fmatrix	m2b_transform;	// model to bone conversion transform
	SBoneShape shape;
	shared_str game_mtl_name;
	u16	game_mtl_idx;
	SJointIKData IK_data;
	float mass;
	Fvector center_of_mass;

	vecBones children;		// bones which are slaves to this

	using FacesVec = xr_vector<u16>;
	using ChildFacesVec = xr_vector<FacesVec>;
	ChildFacesVec child_faces;	// shared
public:
	CBoneData(u16 ID) :SelfID(ID) { VERIFY(SelfID != BI_NONE); }
	virtual	~CBoneData() {}

	typedef svector<int, 128> BoneDebug;
	void DebugQuery(BoneDebug& L);

	inline void				SetParentID(u16 id) { ParentID = id; }
	inline u16 __stdcall GetSelfID() const { return SelfID; }
	inline u16 __stdcall GetParentID() const { return ParentID; }

	// assign face
	void AppendFace(u16 child_idx, u16 idx)
	{
		child_faces[child_idx].push_back(idx);
	}
	// Calculation
	void				CalculateM2B(const Fmatrix& Parent);
private:
	CBoneData & __stdcall GetChild(u16 id);
	const CBoneData& __stdcall GetChild(u16 id) const;
	u16	__stdcall GetNumChildren() const;

	inline const SJointIKData& __stdcall get_IK_data() const { return IK_data; }
	inline const Fmatrix& __stdcall get_bind_transform() const { return	bind_transform; }
	inline const SBoneShape&	__stdcall get_shape() const { return shape; }
	inline const Fobb& __stdcall get_obb() const { return obb; }
	inline const Fvector& __stdcall get_center_of_mass() const { return center_of_mass; }
	inline float __stdcall	get_mass() const { return mass; }
	inline u16	__stdcall	get_game_mtl_idx() const { return game_mtl_idx; }
	inline float __stdcall	lo_limit(u8 k) const { return IK_data.limits[k].limit.x; }
	inline float __stdcall	hi_limit(u8 k) const { return IK_data.limits[k].limit.y; }
public:
	virtual u32	mem_usage()
	{
		u32 sz = sizeof(*this) + sizeof(vecBones::value_type)*u32(children.size());

		for (auto c_it = child_faces.begin(); c_it != child_faces.end(); ++c_it)
			sz += u32(c_it->size() * sizeof(FacesVec::value_type) + sizeof(*c_it));

		return sz;
	}
};
