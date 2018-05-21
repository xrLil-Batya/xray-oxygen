#pragma once
class IPhysicsShellEx;
class CCameraBase;
class IPhysicsShellHolder;
extern XRPHYSICS_API IPhysicsShellEx*	actor_camera_shell;
extern XRPHYSICS_API BOOL dbg_draw_camera_collision;
extern XRPHYSICS_API float	camera_collision_character_skin_depth;
extern XRPHYSICS_API float	camera_collision_character_shift_z;
XRPHYSICS_API bool test_camera_box(const Fvector &box_size, const Fmatrix &xform, IPhysicsShellHolder* l_actor);
XRPHYSICS_API void	collide_camera(CCameraBase & camera, float _viewport_near, IPhysicsShellHolder *l_actor);
