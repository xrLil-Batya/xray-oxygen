#include "stdafx.h"
#include "Actor.h"
#include "../xrEngine/CameraBase.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"

#include "items/Weapon.h"
#include "Inventory.h"

#include "SleepEffector.h"
#include "ActorEffector.h"
#include "level.h"
#include "../xrCDB/cl_intersect.h"

//#include "elevatorstate.h"
#include "CharacterPhysicsSupport.h"
#include "EffectorShot.h"

#include "PHMovementControl.h"
#include "../xrphysics/ielevatorstate.h"
#include "../xrphysics/actorcameracollision.h"
#include "IKLimbsController.h"
#include "GamePersistent.h"

ENGINE_API extern float psHUD_FOV;
ENGINE_API extern float psHUD_FOV_def;

float CActor::f_Ladder_cam_limit=1.f;
void CActor::cam_SetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	g_LadderOrient			();
	float yaw				= (-XFORM().k.getH());
	float &cam_yaw			= C->yaw;
	float delta_yaw			= angle_difference_signed(yaw,cam_yaw);

	if(-f_Ladder_cam_limit<delta_yaw&&f_Ladder_cam_limit>delta_yaw)
	{
		yaw					= cam_yaw+delta_yaw;
		float lo			= (yaw-f_Ladder_cam_limit);
		float hi			= (yaw+f_Ladder_cam_limit);
		C->lim_yaw[0]		= lo;
		C->lim_yaw[1]		= hi;
		C->bClampYaw		= true;
	}
}
void CActor::camUpdateLadder(float dt)
{
	if(!character_physics_support()->movement()->ElevatorState())
															return;
	if(cameras[eacFirstEye]->bClampYaw) return;
	float yaw				= (-XFORM().k.getH());

	float & cam_yaw			= cameras[eacFirstEye]->yaw;
	float delta				= angle_difference_signed(yaw,cam_yaw);

	if(-0.05f<delta&&0.05f>delta)
	{
		yaw									= cam_yaw+delta;
		float lo							= (yaw-f_Ladder_cam_limit);
		float hi							= (yaw+f_Ladder_cam_limit);
		cameras[eacFirstEye]->lim_yaw[0]	= lo;
		cameras[eacFirstEye]->lim_yaw[1]	= hi;
		cameras[eacFirstEye]->bClampYaw		= true;
	}else{
		cam_yaw								+= delta * std::min(dt*10.f,1.f) ;
	}

	IElevatorState* es = character_physics_support()->movement()->ElevatorState();
	if(es && es->State()==clbClimbingDown)
	{
		float &cam_pitch					= cameras[eacFirstEye]->pitch;
		const float ldown_pitch				= cameras[eacFirstEye]->lim_pitch.y;
		float delta							= angle_difference_signed(ldown_pitch,cam_pitch);
		if(delta>0.f)
			cam_pitch						+= delta* std::min(dt*10.f,1.f) ;
	}
}

void CActor::cam_UnsetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	C->lim_yaw[0]			= 0;
	C->lim_yaw[1]			= 0;
	C->bClampYaw			= false;
}
float cammera_into_collision_shift = 0.05f;
float CActor::CameraHeight()
{
	Fvector						R;
	character_physics_support()->movement()->Box().getsize		(R);
	return						m_fCamHeightFactor*( R.y - cammera_into_collision_shift );
}

IC float viewport_near(float& w, float& h)
{
	w = 2.f*VIEWPORT_NEAR*tan(deg2rad(Device.fFOV)/2.f);
	h = w*Device.fASPECT;
	float	c	= _sqrt					(w*w + h*h);
	return	std::max(std::max(VIEWPORT_NEAR, std::max(w,h)),c);
}

ICF void calc_point(Fvector& pt, float radius, float depth, float alpha)
{
	pt.x	= radius*_sin(alpha);
	pt.y	= radius+radius*_cos(alpha);
	pt.z	= depth;
}
ICF void calc_gl_point(Fvector& pt, const Fmatrix& xform, float radius, float angle )
{
	calc_point			(pt,radius,VIEWPORT_NEAR/2,angle);
	xform.transform_tiny(pt);
}
ICF BOOL test_point( const Fvector	&pt, xrXRC& xrc,  const Fmatrix33& mat, const Fvector& ext )
{
	for (auto it = xrc.r_realBegin(); it != xrc.r_realEnd(); it++)	{
		CDB::RESULT&	O	= *it;
		if ( GMLib.GetMaterialByIdx(O.material)->Flags.is(SGameMtl::flPassable) )
			continue;
		if ( CDB::TestBBoxTri(mat,pt,ext,O.verts,false) )
			return		TRUE;
	}
	return FALSE;
}


IC bool test_point( const Fvector	&pt, const Fmatrix33& mat, const Fvector& ext, CActor* actor  )
{
	Fmatrix fmat = Fidentity;
	fmat.i.set( mat.i );
	fmat.j.set( mat.j );
	fmat.k.set( mat.k );
	fmat.c.set( pt );

	VERIFY( actor );
	return test_camera_box( ext, fmat, actor );
}

IC void get_box_mat( Fmatrix33	&mat, float alpha, const SRotation	&r_torso  )
{
	float dZ			= ((PI_DIV_2-((PI+alpha)/2)));
	Fmatrix				xformR;
	xformR.setXYZ		(-r_torso.pitch,r_torso.yaw,-dZ);
	mat.i				= xformR.i;
	mat.j				= xformR.j;
	mat.k				= xformR.k;
}
IC void get_q_box( Fbox &xf,  float c, float alpha, float radius )
{
	Fvector src_pt,		tgt_pt;
	calc_point			(tgt_pt,radius,0,alpha);
	src_pt.set			(0,tgt_pt.y,0);
	xf.invalidate		();
	xf.modify			(src_pt);
	xf.modify			(tgt_pt);
	xf.grow				(c);

}

IC void get_cam_oob( Fvector	&bc, Fvector &bd, Fmatrix33	&mat, const Fmatrix &xform, const SRotation &r_torso, float alpha, float radius, float c )
{
	get_box_mat ( mat, alpha, r_torso );
	Fbox				xf;
	get_q_box( xf, c, alpha, radius );
	xf.xform			( Fbox().set(xf), xform )	;
	// query
	xf.get_CD			(bc,bd)		;
}
IC void get_cam_oob(  Fvector &bd, Fmatrix	&mat, const Fmatrix &xform, const SRotation &r_torso, float alpha, float radius, float c )
{
	
	Fmatrix33	mat3;
	Fvector		bc;
	get_cam_oob( bc, bd, mat3, xform, r_torso, alpha, radius, c );
	mat.set( Fidentity );
	mat.i.set( mat3.i );
	mat.j.set( mat3.j );
	mat.k.set( mat3.k );
	mat.c.set( bc );

}
void	CActor::cam_Lookout	( const Fmatrix &xform, float camera_height )
{
	if (!fis_zero(r_torso_tgt_roll))
	{
		
		float w,h;
		float c				= viewport_near(w,h); w/=2.f;h/=2.f;
		float alpha			= r_torso_tgt_roll/2.f;
		float radius		= camera_height*0.5f;
		// init valid angle
		float valid_angle	= alpha;
		Fvector				bc,bd;
		Fmatrix33			mat;
		get_cam_oob( bc, bd, mat, xform, r_torso, alpha, radius, c );

		/*
		xrXRC				xrc			;
		xrc.box_options		(0)			;
		xrc.box_query		(Level().ObjectSpace.GetStaticModel(), bc, bd)		;
		u32 tri_count		= xrc.r_count();

		*/
		//if (tri_count)		
		{
			float da		= 0.f;
			BOOL bIntersect	= FALSE;
			Fvector	ext		= {w,h,VIEWPORT_NEAR/2};
			Fvector				pt;
			calc_gl_point	( pt, xform, radius, alpha );
			if ( test_point( pt, mat, ext, this  ) )
			{
				da			= PI/1000.f;
				if (!fis_zero(r_torso.roll))
					da		*= r_torso.roll/_abs(r_torso.roll);

                float angle = 0.f;
				for (; _abs(angle)<_abs(alpha); angle+=da)
				{
					Fvector				pt;
					calc_gl_point( pt, xform, radius, angle );
					if (test_point( pt, mat,ext, this )) 
						{ bIntersect=TRUE; break; } 
				}
				valid_angle	= bIntersect?angle:alpha;
			} 
		}
		r_torso.roll		= valid_angle*2.f;
		r_torso_tgt_roll	= r_torso.roll;
	}
	else
	{	
		r_torso_tgt_roll = 0.f;
		r_torso.roll = 0.f;
	}
}
#ifdef	DEBUG
BOOL ik_cam_shift = true;
float ik_cam_shift_tolerance = 0.2f;
float ik_cam_shift_speed = 0.01f;
#else
static const BOOL	ik_cam_shift = true;
static const float	ik_cam_shift_tolerance = 0.2f;
static const float	ik_cam_shift_speed = 0.01f;
#endif
#include "../xrEngine/xr_input.h"
#include "visual_memory_manager.h"
#include "actor_memory.h"
#include "../Include/xrRender/Kinematics.h"
#include "relation_registry.h"
#include "character_info.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/monsters/basemonster/base_monster.h"
#include <ai/monsters/poltergeist/poltergeist.h>
int cam_dik = VK_SHIFT;
Fvector vel = { 0, 0, 0 };
ref_light enemy_spotlight2;
static shared_str pname = "_samples_particles_\\orbit_point_01";
CObject*		m_locked_enemy = nullptr;
Fvector coords = { 0.23f, 0.1f, 0.4f };

void UpdateAutoAim()
{

	Fmatrix transform;
	u16 head_bone;//kinematics->LL_BoneID("bip01_head");
	CEntityAlive* Enemy_Alive = smart_cast<CEntityAlive*>(m_locked_enemy);

	CPoltergeist* Polt = smart_cast<CPoltergeist*> (m_locked_enemy);
	if (Enemy_Alive->cast_base_monster())
	{

		head_bone = m_locked_enemy->Visual()->dcast_PKinematics()->LL_BoneID(Enemy_Alive->cast_base_monster()->get_head_bone_name());

	}
	else
		head_bone = m_locked_enemy->Visual()->dcast_PKinematics()->LL_BoneID("bip01_head");

	m_locked_enemy->Visual()->dcast_PKinematics()->Bone_GetAnimPos(transform, u16(head_bone), u8(-1), false);

	CBoneInstance& BI = smart_cast<IKinematics*>(m_locked_enemy->Visual())->LL_GetBoneInstance(head_bone);
	Fmatrix M;
	M.mul(m_locked_enemy->XFORM(), BI.mTransform);
	enemy_spotlight2->set_rotation(Fvector().set(0.1f, -180.f, 0.f), M.i);

	Fvector result;

	transform.transform_tiny(result, Fvector().set(0.1f, 0.f, 0.f));

	m_locked_enemy->XFORM().transform_tiny(result, Fvector(result));



	enemy_spotlight2->set_position(Fvector().set(m_locked_enemy->XFORM().c.x, result.y + 1.0f, m_locked_enemy->XFORM().c.z));
	Fvector _dest_dir;

	_dest_dir.sub(result, Actor()->cam_Active()->vPosition);

	Fmatrix _m;
	_m.identity();
	_m.k.normalize_safe(_dest_dir);
	Fvector::generate_orthonormal_basis(_m.k, _m.j, _m.i);

	Fvector xyz;
	_m.getXYZi(xyz);
	
	Actor()->cam_Active()->yaw = angle_inertion_var(Actor()->cam_Active()->yaw, xyz.y,
		100.f,
		100.f,
		PI,
		Device.fTimeDelta);

	Actor()->cam_Active()->pitch = angle_inertion_var(Actor()->cam_Active()->pitch, xyz.x,
		100.f,
		100.f,
		PI,
		Device.fTimeDelta);
}



void CActor::cam_Update(float dt, float fFOV)
{
	if (m_holder)
		return;

	// HUD FOV Update
	if (this == Level().CurrentControlEntity())
	{
		CWeapon* pWeapon = smart_cast<CWeapon*>(this->inventory().ActiveItem());
		if (pWeapon)
			psHUD_FOV = pWeapon->GetHudFov();
		else
			psHUD_FOV = psHUD_FOV_def;
	}

	if ((mstate_real & mcClimb) && (cam_active != eacFreeLook))
		camUpdateLadder(dt);
	on_weapon_shot_update();
	float y_shift = 0;
	current_ik_cam_shift = 0;

	// Alex ADD: smooth crouch fix
	float HeightInterpolationSpeed = 4.f;

	if (CurrentHeight != CameraHeight())
	{
		CurrentHeight = (CurrentHeight * (1.0f - HeightInterpolationSpeed * dt)) + (CameraHeight() * HeightInterpolationSpeed*dt);
	}

	Fvector point = { 0, CurrentHeight + current_ik_cam_shift, 0 };
	Fvector dangle = { 0,0,0 };
	Fmatrix				xform;
	xform.setXYZ(0, r_torso.yaw, 0);
	xform.translate_over(XFORM().c);

	// lookout
	if (this == Level().CurrentControlEntity())
		cam_Lookout(xform, point.y);


	if (!fis_zero(r_torso.roll))
	{
		float radius = point.y*0.5f;
		float valid_angle = r_torso.roll / 2.f;
		calc_point(point, radius, 0, valid_angle);
		dangle.z = (PI_DIV_2 - ((PI + valid_angle) / 2));
	}

	float flCurrentPlayerY = xform.c.y;

	// Smooth out stair step ups
	if ((character_physics_support()->movement()->Environment() == CPHMovementControl::peOnGround) && (flCurrentPlayerY - fPrevCamPos > 0)) {
		fPrevCamPos += dt * 1.5f;
		if (fPrevCamPos > flCurrentPlayerY)
			fPrevCamPos = flCurrentPlayerY;
		if (flCurrentPlayerY - fPrevCamPos > 0.2f)
			fPrevCamPos = flCurrentPlayerY - 0.2f;
		point.y += fPrevCamPos - flCurrentPlayerY;
	}
	else {
		fPrevCamPos = flCurrentPlayerY;
	}

	float _viewport_near = VIEWPORT_NEAR;
	// calc point
	xform.transform_tiny(point);

	CCameraBase* C = cam_Active();

	if (psActorFlags.test(AF_BINDED_CAMERA))
	{
		if (mstate_real & mcLLookout && cam_active == eacLookAt)
		{
			coords.y = .5f;
			coords.x = .5f;
		}
		else
		{
			coords.y = 0.1f;
			coords.x = 0.23f;
		}

		IKinematics*  kinematics = smart_cast<IKinematics*>(Visual());

		Fvector result;

		kinematics->LL_GetTransform(u16(m_head)).transform_tiny(result, coords);

		XFORM().transform_tiny(result, Fvector(result));

		kinematics->CalculateBones_Invalidate();

		C->Update(result, dangle);
	}
	else
		C->Update(point, dangle);

	if (psActorFlags.test(AF_AIM_ASSIST))
	{
		enemy_spotlight2 = ::Render->light_create(); enemy_spotlight2->set_active(0);
		enemy_spotlight2->set_type(IRender_Light::SPOT);
		enemy_spotlight2->set_shadow(true);
		enemy_spotlight2->set_cone(deg2rad(30.f));
		enemy_spotlight2->set_color(Fcolor().set(5.0f, 2.0f, 0.0f, 25.0f));
		enemy_spotlight2->set_range(4.0f);



		CEntityAlive* Enemy_Alive = smart_cast<CEntityAlive*>(m_locked_enemy);
		CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(this);
		CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(smart_cast<CAI_Stalker*>(Enemy_Alive));
		CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(Enemy_Alive);
		CPoltergeist* Polt = smart_cast<CPoltergeist*> (m_locked_enemy);
		float aldist = ai().alife().switch_distance();

		if (!m_locked_enemy)
		{
			if (pInput->iGetAsyncKeyState(cam_dik))
			{

				const CVisualMemoryManager::VISIBLES& vVisibles = memory().visual().objects();
				CVisualMemoryManager::VISIBLES::const_iterator v_it = vVisibles.begin();
				float nearest_dst = flt_max;

				for (; v_it != vVisibles.end(); ++v_it)
				{
					const CObject* _object_ = (*v_it).m_object;
					CObject* object_ = const_cast<CObject*>(_object_);

					CEntityAlive* EA = smart_cast<CEntityAlive*>(object_);
					if (!EA || !EA->g_Alive() || (Polt && !EA->getVisible()))
						continue;

					if (!memory().visual().visible_right_now(smart_cast<const CGameObject*>(_object_)))
						continue;

					float dst = object_->Position().distance_to_xz(Position());
					if (!m_locked_enemy || dst < nearest_dst)
					{
						m_locked_enemy = object_;
						nearest_dst = dst;
					}
				}
				//.			if (m_locked_enemy)
				//.				Msg("enemy is %s", *m_locked_enemy->cNameSect());
			}
		}
		else if (!Enemy_Alive || !Enemy_Alive->getEnabled() || !Enemy_Alive->g_Alive() || Enemy_Alive->Position().distance_to(Actor()->Position()) > aldist - 10.0f)//|| (!Enemy_Alive->cast_base_monster() && SRelationRegistry().GetRelationType(others_inv_owner, our_inv_owner) != ALife::eRelationTypeEnemy))
		{

			m_locked_enemy = nullptr;
		}
		else
		{
			if (!pInput->iGetAsyncKeyState(cam_dik))
			{
				m_locked_enemy = nullptr;
				enemy_spotlight2->set_active(0);
				if (PP && PP->IsPlaying() || m_locked_enemy == nullptr)
					PP->StopParticles(pname, BI_NONE, false);
			}
			else
			{
				enemy_spotlight2->set_active(1);


				if (Enemy_Alive->PPhysicsShell())
					Enemy_Alive->PHGetLinearVell(vel);
				if (PP)
				{
					PP->StartParticles(pname, Fvector().set(0.0f, 1.0f, 0.0f), m_locked_enemy->ID(), -1, false);
					PP->SetParentVel(vel);
				}
				UpdateAutoAim();
			}
		}
		if (m_locked_enemy == nullptr)
		{
			enemy_spotlight2->set_active(0);
			if (PP && PP->IsPlaying() && m_locked_enemy == nullptr)
				PP->StopParticles(pname, BI_NONE, false);
		}
	}

	C->f_fov = fFOV;

	if (Level().CurrentEntity() == this)
	{
		collide_camera(*cameras[eacFirstEye], _viewport_near, this);
	}

	Cameras().UpdateFromCamera(C);

	fCurAVelocity = vPrevCamDir.sub(cameras[eacFirstEye]->vDirection).magnitude() / Device.fTimeDelta;
	vPrevCamDir = cameras[eacFirstEye]->vDirection;

	if (Level().CurrentEntity() == this)
	{
		Level().Cameras().UpdateFromCamera(C);

		const bool allow = !Level().Cameras().GetCamEffector(cefDemo) && !Level().Cameras().GetCamEffector(cefAnsel);
		if (eacFirstEye == cam_active && allow)
			Cameras().ApplyDevice(_viewport_near);
	}
}

// shot effector stuff
void CActor::update_camera (CCameraShotEffector* effector)
{
	if (!effector) return;
	//	if (Level().CurrentViewEntity() != this) return;

	CCameraBase* pACam = cam_Active();
	if (!pACam) return;

	if (pACam->bClampPitch)
	{
		while (pACam->pitch < pACam->lim_pitch[0])
			pACam->pitch += PI_MUL_2;
		while (pACam->pitch > pACam->lim_pitch[1])
			pACam->pitch -= PI_MUL_2;
	}

	effector->ChangeHP( &(pACam->pitch), &(pACam->yaw) );

	if (pACam->bClampYaw)	clamp(pACam->yaw,pACam->lim_yaw[0],pACam->lim_yaw[1]);
	if (pACam->bClampPitch)	clamp(pACam->pitch,pACam->lim_pitch[0],pACam->lim_pitch[1]);

	if (effector && !effector->IsActive())
	{
		Cameras().RemoveCamEffector(eCEShot);
	}
}


#ifdef DEBUG
void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
extern	Flags32	dbg_net_Draw_Flags;
extern	BOOL g_bDrawBulletHit;

void CActor::OnRender	()
{
	if (inventory().ActiveItem())
		inventory().ActiveItem()->OnRender();

	if (!bDebug)				return;

	if ((dbg_net_Draw_Flags.is_any(dbg_draw_actor_phys)))
		character_physics_support()->movement()->dbg_Draw	();

	inherited::OnRender();
}
#endif
