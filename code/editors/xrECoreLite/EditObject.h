#pragma once
#include "GameMtlLib.h"
#if !defined(_LW_PLUGIN) && !defined(NT_PLUGIN)
#include "xrCDB/xrCDB.h"
#include "xrSound/Sound.h"
#endif
#include "bone.h"
#include "motion.h"

#if !defined(_LW_PLUGIN) && !defined(NT_PLUGIN)
#include "xrEngine/GameMtlLib.h"
#endif
#ifdef _EDITOR
#	include "../../../xrServerEntities/PropertiesListTypes.h"
#	include "pick_defs.h"
#endif
//----------------------------------------------------
struct SRayPickInfo;
class CEditableMesh;
class CFrustum;
class CCustomMotion;
class CBone;
struct Shader;
class Mtl;
class CExporter;
class CMayaTranslator;
struct st_ObjectDB;
struct SXRShaderData;
struct ogf_desc;
class CCustomObject;

#ifndef _EDITOR
class PropValue;
#define ref_shader LPVOID
#endif

#ifdef _LW_IMPORT
#include <lwobjimp.h>
#include <lwsurf.h>
#endif

#define LOD_SHADER_NAME "details\\lod"
#define LOD_SAMPLE_COUNT 8
#define LOD_IMAGE_SIZE 64
#define RENDER_SKELETON_LINKS 4

// refs
class XRayMtl;
class SSimpleImage;

class ECORE_API CSurface
{
	u32 m_GameMtlID;
	ref_shader m_Shader;

	enum ERTFlags
	{
		rtValidShader = (1 << 0),
	};

public:
	enum EFlags
	{
		sf2Sided = (1 << 0),
	};
	shared_str m_Name;
	shared_str m_Texture;	//
	shared_str m_VMap;		//
	shared_str m_ShaderName;
	shared_str m_ShaderXRLCName;
	shared_str m_GameMtlName;
	Flags32 m_Flags;
	u32 m_dwFVF;

#ifdef _MAX_EXPORT
	u32 mid;
	Mtl *mtl;
#endif
#ifdef _LW_IMPORT
	LWSurfaceID surf_id;
#endif
	Flags32 m_RTFlags;
	u32 tag;
	SSimpleImage *m_ImageData;
public:
	CSurface()
	{
		m_GameMtlName = "default";
		m_ImageData = 0;
		m_Shader = 0;
		m_RTFlags.zero();
		m_Flags.zero();
		m_dwFVF = 0;
#ifdef _MAX_EXPORT
		mtl = 0;
		mid = 0;
#endif

#ifdef _LW_IMPORT
		surf_id = 0;
#endif

		tag = 0;
	}

	inline bool Validate() { return (0 != xr_strlen(m_Texture)) && (0 != xr_strlen(m_ShaderName)); }
#ifdef _EDITOR
	~CSurface() { R_ASSERT(!m_Shader); xr_delete(m_ImageData); }
	inline void CopyFrom(CSurface* surf) { *this = *surf; m_Shader = 0; }
	inline int _Priority() { return _Shader() ? _Shader()->E[0]->flags.iPriority : 1; }
	inline bool _StrictB2F() { return _Shader() ? _Shader()->E[0]->flags.bStrictB2F : false; }
	inline ref_shader _Shader() { if (!m_RTFlags.is(rtValidShader)) OnDeviceCreate(); return m_Shader; }
#endif
	inline const char* _Name()const { return *m_Name; }
	inline const char* _ShaderName()const { return *m_ShaderName; }
	inline const char* _GameMtlName()const { return *m_GameMtlName; }
	inline const char* _ShaderXRLCName()const { return *m_ShaderXRLCName; }
	inline const char* _Texture()const { return *m_Texture; }
	inline const char* _VMap()const { return *m_VMap; }
	inline u32 _FVF()const { return m_dwFVF; }
	inline void SetName(const char* name) { m_Name = name; }
	inline void SetShader(const char* name)
	{
		R_ASSERT2(name&&name[0], "Empty shader name.");
		m_ShaderName = name;
#ifdef _EDITOR
		OnDeviceDestroy();
#endif
	}
	inline void SetShaderXRLC(const char* name) { m_ShaderXRLCName = name; }
	inline void SetGameMtl(const char* name) { m_GameMtlName = name; }
	inline void SetFVF(u32 fvf) { m_dwFVF = fvf; }
	inline void SetTexture(const char* name) { string512 buf; strcpy(buf, name); if (strext(buf)) *strext(buf) = 0; m_Texture = buf; }
	inline void SetVMap(const char* name) { m_VMap = name; }
	inline u32 _GameMtl()const { return GEMLib.GetMaterialID(*m_GameMtlName); }
#ifdef _EDITOR
	inline void OnDeviceCreate()
	{
		R_ASSERT(!m_RTFlags.is(rtValidShader));

		if (m_ShaderName.size() && m_Texture.size())
			m_Shader.create(*m_ShaderName, *m_Texture);
		else
			m_Shader.create("editor\\wire");

		m_RTFlags.set(rtValidShader, TRUE);
	}
	inline void OnDeviceDestroy()
	{
		m_Shader.destroy();
		m_RTFlags.set(rtValidShader, FALSE);
	}
	void CreateImageData();
	void RemoveImageData();
#endif
};

using SurfaceVec = xr_vector<CSurface*>;
using EditMeshVec = xr_vector<CEditableMesh*>;
using OMotionVec = xr_vector<COMotion*>;
using SMotionVec = xr_vector<CSMotion*>;

struct ECORE_API SBonePart
{
	shared_str alias;
	RStringVec bones;
};
using BPVec = xr_vector<SBonePart>;
#include "files_list.hpp"
const u32 FVF_SV = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL;

class ECORE_API CEditableObject
{
	friend class CSceneObject;
	friend class CEditableMesh;
	friend class TfrmPropertiesEObject;
	friend class CSector;
	friend class TUI_ControlSectorAdd;
	friend class ELibrary;
	friend class TfrmEditLibrary;
	friend class MeshExpUtility;

#ifdef _EDITOR
	ref_geom vs_SkeletonGeom;
#endif
	// desc
	shared_str m_CreateName;
	__time32_t m_CreateTime;
	shared_str m_ModifName;
	__time32_t m_ModifTime;

	// general
	xr_string m_ClassScript;

	ref_shader m_LODShader;

	// skeleton
	BPVec m_BoneParts;
	CSMotion *m_ActiveSMotion;
public:// skeleton
	BoneVec m_Bones;
	SMotionVec m_SMotions;
	//Etc
	SurfaceVec m_Surfaces;
	EditMeshVec m_Meshes;
	SAnimParams m_SMParam;
	xr_vector<shared_str> m_SMotionRefs;
	shared_str m_LODs;
public:
	// options
	Flags32 m_objectFlags;
	enum {
		eoDynamic = (1 << 0),
		eoProgressive = (1 << 1),
		eoUsingLOD = (1 << 2),
		eoHOM = (1 << 3),
		eoMultipleUsage = (1 << 4),
		eoSoundOccluder = (1 << 5),
		eoFORCE32 = u32(-1)
	};
	inline BOOL IsDynamic() { return m_objectFlags.is(eoDynamic); }
	inline BOOL IsStatic() { return !m_objectFlags.is(eoSoundOccluder) && !m_objectFlags.is(eoDynamic) && !m_objectFlags.is(eoHOM) && !m_objectFlags.is(eoMultipleUsage); }
	inline BOOL IsMUStatic() { return !m_objectFlags.is(eoSoundOccluder) && !m_objectFlags.is(eoDynamic) && !m_objectFlags.is(eoHOM) && m_objectFlags.is(eoMultipleUsage); }

	shared_str GetCreatorName()			const { return m_CreateName;  }
	shared_str GetModificatorName()		const { return m_ModifName;   }

	__time32_t GetCreationTime()		const { return m_CreateTime; }
	__time32_t GetModificationTime()	const { return m_ModifTime; }

private:
	// bounding volume
	Fbox m_Box;
public:
	// temp variable for actor
	Fvector a_vPosition;
	Fvector a_vRotate;

	// temp variables for transformation
	Fvector t_vPosition;
	Fvector t_vScale;
	Fvector t_vRotate;

	bool bOnModified;
	inline bool IsModified() { return bOnModified; }
	inline void Modified() { bOnModified = true; }

	xr_string m_LoadName;
	int m_RefCount;
protected:
	int m_ObjectVersion;

	void ClearGeometry();

	void PrepareBones();
	void DefferedLoadRP();
	void DefferedUnloadRP();

	void __stdcall  OnChangeTransform(PropValue* prop);
	void __stdcall 	OnChangeShader(PropValue* prop);
public:
	enum {
		LS_RBUFFERS = (1 << 0),
		//	    LS_GEOMETRY	= (1<<1),
	};
	Flags32 m_LoadState;

	xr_string m_LibName;
public:
	// constructor/destructor methods
	CEditableObject(const char* name);
	virtual ~CEditableObject();

	const char* GetName() { return m_LibName.c_str(); }

	void SetVersionToCurrent(BOOL bCreate, BOOL bModif);

	void Optimize();

	inline EditMeshVec& Meshes() { return m_Meshes; }
	inline EditMeshVec::iterator FirstMesh() { return m_Meshes.begin(); }
	inline int MeshCount() { return int(m_Meshes.size()); }
	inline void AppendMesh(CEditableMesh* M) { m_Meshes.push_back(M); }
	inline SurfaceVec& Surfaces() { return m_Surfaces; }
	inline SurfaceVec::iterator FirstSurface() { return m_Surfaces.begin(); }
	inline int SurfaceCount() { return int(m_Surfaces.size()); }
	inline int Version() { return m_ObjectVersion; }

	// LOD
	xr_string GetLODTextureName();
	const char* GetLODShaderName() { return LOD_SHADER_NAME; }
	void GetLODFrame(int frame, Fvector p[4], Fvector2 t[4], const Fmatrix* parent = 0);

	// skeleton
	inline BPVec& BoneParts() { return m_BoneParts; }
	inline u32 BonePartCount() { return u32(m_BoneParts.size()); }
	inline BoneVec& Bones() { return m_Bones; }
	inline int BoneCount() { return int(m_Bones.size()); }
	shared_str BoneNameByID(int id);
	int GetRootBoneID();
	int PartIDByName(const char* name);
	inline CBone* GetBone(u32 idx) { VERIFY(idx < u32(m_Bones.size())); return (CBone*)m_Bones[idx]; }
	void GetBoneWorldTransform(u32 bone_idx, float t, CSMotion* motion, Fmatrix& matrix);
	SMotionVec& SMotions() { return m_SMotions; }
	inline u32 SMotionCount() { return (u32)m_SMotions.size(); }
	inline bool IsAnimated() { return SMotionCount() || m_SMotionRefs.size(); }
	inline void SkeletonPlay() { m_SMParam.Play(); }
	inline void SkeletonStop() { m_SMParam.Stop(); }
	inline void SkeletonPause(bool val) { m_SMParam.Pause(val); }

	// get object properties methods

	inline xr_string& GetClassScript() { return m_ClassScript; }
	inline const Fbox& GetBox() { return m_Box; }
	inline const char* GetLODs() { return m_LODs.c_str(); }

	// animation
	inline bool IsSkeleton() { return !!m_Bones.size(); }
	inline bool IsSMotionActive() { return IsSkeleton() && m_ActiveSMotion; }
	CSMotion* GetActiveSMotion() { return m_ActiveSMotion; }
	void SetActiveSMotion(CSMotion* mot);
	bool CheckBoneCompliance(CSMotion* M);
	bool VerifyBoneParts();
	void OptimizeSMotions();

	bool LoadBoneData(IReader& F);
	void SaveBoneData(IWriter& F);
	void ResetBones();
	CSMotion* ResetSAnimation(bool bGotoBindPose = true);
	void CalculateAnimation(CSMotion* motion);
	void CalculateBindPose();
	void GotoBindPose();
	void OnBindTransformChange();

	// statistics methods
	void GetFaceWorld(const Fmatrix& parent, CEditableMesh* M, int idx, Fvector* verts);
	int GetFaceCount();
	int GetVertexCount();
	int GetSurfFaceCount(const char* surf_name);

	// render methods
	void Render(const Fmatrix& parent, int priority, bool strictB2F);
	void RenderSelection(const Fmatrix& parent, CEditableMesh* m = 0, CSurface* s = 0, u32 c = 0x40E64646);
	void RenderEdge(const Fmatrix& parent, CEditableMesh* m = 0, CSurface* s = 0, u32 c = 0xFFC0C0C0);
	void RenderBones(const Fmatrix& parent);
	void RenderAnimation(const Fmatrix& parent);
	void RenderSingle(const Fmatrix& parent);
	void RenderSkeletonSingle(const Fmatrix& parent);
	void RenderLOD(const Fmatrix& parent);

	// update methods
	void OnFrame();
	void UpdateBox();
	void EvictObject();

	// pick methods
#ifdef _EDITOR
	bool RayPick(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& inv_parent, SRayPickInfo* pinf = 0);

	void AddBone(CBone* parent_bone);
	void DeleteBone(CBone* bone);
	void RenameBone(CBone* bone, const char* new_name);

	void RayQuery(SPickQuery& pinf);
	void RayQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
	void BoxQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
	bool BoxPick(CCustomObject* obj, const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf);
	bool FrustumPick(const CFrustum& frustum, const Fmatrix& parent);
	bool SpherePick(const Fvector& center, float radius, const Fmatrix& parent);

	// bone
	CBone* PickBone(const Fvector& S, const Fvector& D, const Fmatrix& parent);
	void SelectBones(bool bVal);
	void SelectBone(CBone* b, bool bVal);
	void ClampByLimits(bool bSelOnly);
#endif
	// change position/orientation methods
	void TranslateToWorld(const Fmatrix& parent);

	// clone/copy methods
	void RemoveMesh(CEditableMesh* mesh);

	bool RemoveSMotion(const char* name);
	bool RenameSMotion(const char* old_name, const char* new_name);
	bool AppendSMotion(const char* fname, SMotionVec* inserted = 0);
	void ClearSMotions();
	bool SaveSMotions(const char* fname);

	// load/save methods
	//void 			LoadMeshDef				(FSChunkDef *chunk);
	bool Reload();
	bool Load(const char* fname);
	bool LoadObject(const char* fname);
	bool SaveObject(const char* fname);
	bool Load(IReader&);
	void Save(IWriter&);
	bool Save(const char* fname);
#ifdef _EDITOR
	void FillMotionList(const char* pref, ListItemsVec& items, int modeID);
	void FillBoneList(const char* pref, ListItemsVec& items, int modeID);
	void FillSurfaceList(const char* pref, ListItemsVec& items, int modeID);
	void FillSurfaceProps(CSurface* surf, const char* pref, PropItemVec& items);
	void FillBasicProps(const char* pref, PropItemVec& items);
	void FillSummaryProps(const char* pref, PropItemVec& items);
	bool CheckShaderCompatible();
#endif
	bool Import_LWO(const char* fname, bool bNeedOptimize);

	// contains methods
	CEditableMesh* FindMeshByName(const char* name, CEditableMesh* Ignore = 0);
	void VerifyMeshNames();
	bool ContainsMesh(const CEditableMesh* m);
	CSurface* FindSurfaceByName(const char* surf_name, int* s_id = 0);
	int FindBoneByNameIdx(const char* name);
	auto FindBoneByNameIt(const char* name);
	CBone* FindBoneByName(const char* name);
	int GetSelectedBones(BoneVec& sel_bones);
	u16 GetBoneIndexByWMap(const char* wm_name);
	CSMotion* FindSMotionByName(const char* name, const CSMotion* Ignore = 0);
	void GenerateSMotionName(char* buffer, const char* start_name, const CSMotion* M);
	bool GenerateBoneShape(bool bSelOnly);

	// device dependent routine
	void OnDeviceCreate();
	void OnDeviceDestroy();

	// utils
	void PrepareOGFDesc(ogf_desc& desc);
	// skeleton
	bool PrepareSVGeometry(IWriter& F, u8 infl);
	bool PrepareSVKeys(IWriter& F);
	bool PrepareSVDefs(IWriter& F);
	bool PrepareSkeletonOGF(IWriter& F, u8 infl);
	// rigid
	bool PrepareRigidOGF(IWriter& F, bool gen_tb, CEditableMesh* mesh);
	// ogf
	bool PrepareOGF(IWriter& F, u8 infl, bool gen_tb, CEditableMesh* mesh);
	bool ExportOGF(const char* fname, u8 skl_infl);
	// omf
	bool PrepareOMF(IWriter& F);
	bool ExportOMF(const char* fname);
	// obj
	bool ExportOBJ(const char* name);

	const char*			GenerateSurfaceName(const char* base_name);
#ifdef _MAX_EXPORT
	BOOL ExtractTexName(Texmap *src, LPSTR dest);
	BOOL ParseStdMaterial(StdMat* src, CSurface* dest);
	BOOL ParseMultiMaterial(MultiMtl* src, u32 mid, CSurface* dest);
	BOOL ParseXRayMaterial(XRayMtl* src, u32 mid, CSurface* dest);
	CSurface* CreateSurface(Mtl* M, u32 mat_id);
	bool ImportMAXSkeleton(CExporter* exporter);
#endif

#ifdef _LW_EXPORT
	bool Import_LWO(st_ObjectDB *I);
	Flags32 m_Flags;
#endif

#ifdef _LW_IMPORT
	bool Export_LW(LWObjectImport *local);
#endif

#ifdef _MAYA_EXPORT
	BOOL ParseMAMaterial(CSurface* dest, SXRShaderData& d);
	CSurface* CreateSurface(LPCSTR m_name, SXRShaderData& d);
	CSurface* CreateSurface(MObject shader);
#endif

	bool ExportLWO(const char* fname);

	bool Validate();
};
//----------------------------------------------------
//----------------------------------------------------
#define EOBJ_CURRENT_VERSION 0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_OBJECT_BODY 0x7777
#define EOBJ_CHUNK_VERSION 0x0900
#define EOBJ_CHUNK_REFERENCE 0x0902
#define EOBJ_CHUNK_FLAGS 0x0903
#define EOBJ_CHUNK_SURFACES 0x0905
#define EOBJ_CHUNK_SURFACES2 0x0906
#define EOBJ_CHUNK_SURFACES3 0x0907
#define EOBJ_CHUNK_EDITMESHES 0x0910
#define _EOBJ_CHUNK_LIB_VERSION_ 0x0911 // obsolette
#define EOBJ_CHUNK_CLASSSCRIPT 0x0912
#define EOBJ_CHUNK_BONES 0x0913
//#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_SMOTIONS 0x0916
#define EOBJ_CHUNK_SURFACES_XRLC 0x0918
#define EOBJ_CHUNK_BONEPARTS 0x0919
#define EOBJ_CHUNK_ACTORTRANSFORM 0x0920
#define EOBJ_CHUNK_BONES2 0x0921
#define EOBJ_CHUNK_DESC 0x0922
#define EOBJ_CHUNK_BONEPARTS2 0x0923
#define EOBJ_CHUNK_SMOTIONS2 0x0924
#define EOBJ_CHUNK_LODS 0x0925
#define EOBJ_CHUNK_SMOTIONS3 0x0926
//----------------------------------------------------