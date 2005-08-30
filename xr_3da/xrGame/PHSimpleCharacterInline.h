void CPHSimpleCharacter::UpdateStaticDamage(dContact* c,SGameMtl* tri_material,bool bo1)
{
	const	dReal	*v			=	dBodyGetLinearVel(m_body);
			dReal	norm_prg	=	dFabs(dDOT(v,c->geom.normal));
			dReal	smag		=	dDOT(v,v);
			dReal	plane_pgr	=	_sqrt(smag-norm_prg*norm_prg);
			dReal	mag			=	0.f;
				if(tri_material->Flags.test(SGameMtl::flPassable))
				{
					mag					=	_sqrt(smag)*tri_material->fBounceDamageFactor;
				}
				else
				{
					float				vel_prg;vel_prg=_max(plane_pgr*tri_material->fPHFriction,norm_prg);
					mag					=	(vel_prg)*tri_material->fBounceDamageFactor;
				}
				if(mag>m_collision_damage_info.m_contact_velocity)
				{
  					m_collision_damage_info.m_contact_velocity	=	mag;
					m_collision_damage_info.m_dmc_signum		=	bo1 ? 1.f : -1.f;
					m_collision_damage_info.m_dmc_type			=	SCollisionDamageInfo::ctStatic;
					m_collision_damage_info.m_damege_contact	=	*c;
					m_collision_damage_info.m_object			=	0;
				}
}

void CPHSimpleCharacter::UpdateDynamicDamage(dContact* c,SGameMtl* obj_material,dBodyID b,bool bo1)
{
	const dReal* vel=dBodyGetLinearVel(m_body);
	dReal c_vel;
	dMass m;
	dBodyGetMass(b,&m);

	const dReal* obj_vel=dBodyGetLinearVel(b);
	const dReal* norm=c->geom.normal;
	dReal norm_vel=dDOT(vel,norm);
	dReal norm_obj_vel=dDOT(obj_vel,norm);

	if((bo1&&norm_vel>norm_obj_vel)||
		(!bo1&&norm_obj_vel>norm_vel)
		) return ; 


	dVector3 Pc={vel[0]*m_mass+obj_vel[0]*m.mass,vel[1]*m_mass+obj_vel[1]*m.mass,vel[2]*m_mass+obj_vel[2]*m.mass};
	//dVectorMul(Vc,1.f/(m_mass+m.mass));
	//dVector3 vc_obj={obj_vel[0]-Vc[0],obj_vel[1]-Vc[1],obj_vel[2]-Vc[2]};
	//dVector3 vc_self={vel[0]-Vc[0],vel[1]-Vc[1],vel[2]-Vc[2]};
	//dReal vc_obj_norm=dDOT(vc_obj,norm);
	//dReal vc_self_norm=dDOT(vc_self,norm);

	dReal Kself=norm_vel*norm_vel*m_mass/2.f;
	dReal Kobj=norm_obj_vel*norm_obj_vel*m.mass/2.f;

	dReal Pcnorm=dDOT(Pc,norm);
	dReal KK=Pcnorm*Pcnorm/(m_mass+m.mass)/2.f;
	dReal accepted_energy=Kself*m_collision_damage_factor+Kobj*object_damage_factor-KK;
	//DeltaK=m1*m2*(v1-v2)^2/(2*(m1+m2))
	if(accepted_energy>0.f)
		c_vel=dSqrt(accepted_energy/m_mass*2.f)*obj_material->fBounceDamageFactor;
	else c_vel=0.f;

	if(c_vel>m_collision_damage_info.m_contact_velocity) 
	{
		m_collision_damage_info.m_contact_velocity=c_vel;
		m_collision_damage_info.m_dmc_signum=bo1 ? 1.f : -1.f;
		m_collision_damage_info.m_dmc_type=SCollisionDamageInfo::ctObject;
		m_collision_damage_info.m_damege_contact=*c;
		m_collision_damage_info.m_object=bo1 ? retrieveRefObject(c->geom.g2) : retrieveRefObject(c->geom.g1);
	}
}