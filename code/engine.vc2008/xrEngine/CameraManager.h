#pragma once
#include "CameraDefs.h"
#include "../xrCore/SPPInfo.h"
using EffectorCamVec = xr_list<CEffectorCam*>;
using EffectorPPVec  = xr_vector<CEffectorPP*>;

#ifndef XRCORE_API
#define XRCORE_API __declspec(dllimport)
#endif

#define effCustomEffectorStartID 10000

struct SCamEffectorInfo;
class ENGINE_API CCameraManager {
protected:
    SCamEffectorInfo m_cam_info;

    BENCH_SEC_SCRAMBLEMEMBER1

    EffectorCamVec m_EffectorsCam;
    EffectorCamVec m_EffectorsCam_added_deffered;
    EffectorPPVec m_EffectorsPP;

    bool m_bAutoApply;
    SPPInfo pp_affected;
    void UpdateDeffered();

    BENCH_SEC_SCRAMBLEVTBL1
    virtual void UpdateCamEffectors();
    virtual void UpdatePPEffectors();
    virtual bool ProcessCameraEffector(CEffectorCam* eff);
    void OnEffectorReleased(SBaseEffector* e);

public:
    u32 dbg_upd_frame;

    BENCH_SEC_SCRAMBLEMEMBER2

    void Dump();
    u32 Count() const { return u32(m_EffectorsCam.size() + m_EffectorsCam_added_deffered.size()); }
    CEffectorCam* AddCamEffector(CEffectorCam* ef);
    CEffectorCam* GetCamEffector(ECamEffectorType type);
    void RemoveCamEffector(ECamEffectorType type);

    ECamEffectorType RequestCamEffectorId();
    EEffectorPPType RequestPPEffectorId();
    CEffectorPP* GetPPEffector(EEffectorPPType type);
    CEffectorPP* AddPPEffector(CEffectorPP* ef);
    void RemovePPEffector(EEffectorPPType type);

    IC Fvector		Position	() const { return m_cam_info.p; }
    IC Fvector		Direction	() const { return m_cam_info.d; }
    IC Fvector		Up			() const { return m_cam_info.n; }
    IC Fvector		Right		() const { return m_cam_info.r; }
    IC float		Fov			() const { return m_cam_info.fFov; }
    IC float		Aspect		() const { return m_cam_info.fAspect; }
	IC ECameraStyle Style		() const { return m_cam_info.style; }
	IC CObject*		Parent		() const { return m_cam_info.parent; }

    IC void camera_Matrix(Fmatrix& M) const { M.set(m_cam_info.r, m_cam_info.n, m_cam_info.d, m_cam_info.p); }
    void Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags, ECameraStyle style = cs_forcedword, CObject* parent = nullptr);
    void UpdateFromCamera(const CCameraBase* C);

    void ApplyDevice(float _viewport_near);
    static void ResetPP();

    CCameraManager(bool bApplyOnUpdate);
    virtual ~CCameraManager();
};

ENGINE_API extern float psCamInert;
ENGINE_API extern float psCamSlideInert;
