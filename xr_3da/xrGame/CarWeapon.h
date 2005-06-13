#pragma once
#include "ShootingObject.h"
#include "HudSound.h"

class CCarWeapon :public CShootingObject
{
protected:
	typedef CShootingObject		inheritedShooting;

	void					SetBoneCallbacks	();
	void					ResetBoneCallbacks	();
	virtual	void			FireStart			();
	virtual	void			FireEnd				();
	virtual	void			UpdateFire			();
	virtual	void			OnShot				();
	void					UpdateBarrelDir		();
	virtual const Fvector&	get_CurrentFirePoint();
	virtual const Fmatrix&	get_ParticlesXFORM	();
	
	CPhysicsShellHolder*	m_object;
	bool					m_bActive;
	bool					m_bAutoFire;
public:
	enum{
			eWpnDesiredDir		=1,
			eWpnDesiredPos,
			eWpnActivate,
			eWpnFire,
			eWpnAutoFire,
	};	
							CCarWeapon			(CPhysicsShellHolder* obj);
				virtual		~CCarWeapon			();
	static void __stdcall	BoneCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneCallbackY		(CBoneInstance *B);
				void		Load				(LPCSTR section);
				void		UpdateCL			();
			void			Action				(int id, u32 flags);
			void			SetParam			(int id, Fvector2 val);
			void			SetParam			(int id, Fvector val);
			bool			AllowFire			();
			float			FireDirDiff			();
private:
	u16						m_rotate_x_bone, m_rotate_y_bone, m_fire_bone, m_camera_bone;
	float					m_tgt_x_rot, m_tgt_y_rot, m_cur_x_rot, m_cur_y_rot, m_bind_x_rot, m_bind_y_rot;
	Fvector					m_bind_x, m_bind_y;
	Fvector					m_fire_dir,m_fire_pos;

	Fmatrix					m_i_bind_x_xform, m_i_bind_y_xform, m_fire_bone_xform;
	Fvector2				m_lim_x_rot, m_lim_y_rot; //in bone space
	float					m_min_gun_speed, m_max_gun_speed;
	CCartridge*				m_Ammo;
	float					m_barrel_speed;
	Fvector					m_destEnemyDir;
	bool					m_allow_fire;
	HUD_SOUND				m_sndShot;
};