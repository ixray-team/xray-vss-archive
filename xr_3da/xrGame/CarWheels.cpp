#include "stdafx.h"
#include "car.h"
#include "../skeletoncustom.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCar::WheelHit(float P,s16 element,ALife::EHitType hit_type)
{
	xr_map   <u16,SWheel>::iterator i=m_wheels_map.find(element);
	if(i!=m_wheels_map.end())
	{
		i->second.Hit(P);
		return true;
	}
	else return false;
}
void CCar::SWheel::Init()
{
	if(inited) return;
	BONE_P_PAIR_CIT bone=bone_map.find(bone_id);
	R_ASSERT2(bone->second.element,"No Element was created for wheel-check collision is set");
	bone->second.element->set_DynamicLimits(default_l_limit,default_w_limit*100.f);
	radius=bone->second.element->getRadius();
	R_ASSERT2(bone->second.joint,"No wheel joint was set for a wheel");
	joint=bone->second.joint;
	joint->SetBackRef(&joint);
	R_ASSERT2(dJointGetType(joint->GetDJoint())==dJointTypeHinge2,"No wheel join was set for a wheel, only wheel-joint valid!!!");
	ApplyDriveAxisVelTorque(0.f,0.f);
	inited=true;
}
void CCar::SWheel::ApplyDriveAxisTorque(float torque)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamFMax2,torque);//car->m_axle_friction
}
void CCar::SWheel::ApplyDriveAxisVel(float vel)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamVel2, vel);
}


void CCar::SWheel::ApplyDriveAxisVelTorque(float vel,float torque)
{
	ApplyDriveAxisVel(vel);
	ApplyDriveAxisTorque(torque);

}
void CCar::SWheel::ApplySteerAxisVel(float vel)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamVel, vel);
}

void CCar::SWheel::ApplySteerAxisTorque(float torque)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamFMax, torque);
}

void CCar::SWheel::ApplySteerAxisVelTorque(float vel,float torque)
{
	ApplySteerAxisVel(vel);
	ApplySteerAxisTorque(torque);
}

void CCar::SWheel::SetSteerHiLimit(float hi)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamHiStop, hi);
}
void CCar::SWheel::SetSteerLoLimit(float lo)
{
	if(!joint) return;
	dJointSetHinge2Param(joint->GetDJoint(), dParamLoStop, lo);
}
void CCar::SWheel::SetSteerLimits(float hi,float lo)
{
	SetSteerHiLimit(hi);
	SetSteerLoLimit(lo);
}

void CCar::SWheel::ApplyDamage(u16 level)
{
	inherited::ApplyDamage(level);
	if(!joint) return;
	if(level == 0 )return;
	float sf,df;
	dJointID dj=joint->GetDJoint();
	switch(level) {

	case 1:
		joint->GetJointSDfactors(sf,df);
		sf/=20.f;df*=4.f;
		joint->SetJointSDfactors(sf,df);
		car->m_damage_particles.PlayWheel1(car,bone_id);
		break;
	case 2:
		
		dVector3 v;
	
		dJointGetHinge2Axis2(dj,v);
		v[0]+=0.1f;v[1]+=0.1f;v[2]+=0.1f;
		dNormalize3(v);
		dJointSetHinge2Axis2(dj,v[0],v[1],v[2]);
		joint->GetJointSDfactors(sf,df);
		sf/=30.f;df*=8.f;
		joint->SetJointSDfactors(sf,df);
		car->m_damage_particles.PlayWheel2(car,bone_id);
		break;
	default: NODEFAULT;
	}
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelDrive::Init()
{
	pwheel->Init();
	gear_factor=pwheel->radius/pwheel->car->m_ref_radius;
	CBoneData& bone_data= smart_cast<CKinematics*>(pwheel->car->Visual())->LL_GetData(u16(pwheel->bone_id));
	switch(bone_data.IK_data.type)
	{
	case jtWheel:
		pos_fvd=bone_map.find(pwheel->bone_id)->second.element->mXFORM.k.x;
		break;

	default: NODEFAULT;
	}

	pos_fvd=pos_fvd>0.f ? -1.f : 1.f;

}
void CCar::SWheelDrive::Drive()
{
	float cur_speed=pwheel->car->RefWheelMaxSpeed()/gear_factor;
	pwheel->ApplyDriveAxisVel(pos_fvd*cur_speed);
	(cur_speed<0.f) ? (cur_speed=-cur_speed) :cur_speed;
	pwheel->ApplyDriveAxisTorque(pwheel->car->RefWheelCurTorque()/gear_factor);
}
void CCar::SWheelDrive::UpdatePower()
{
	float cur_speed=pwheel->car->RefWheelMaxSpeed()/gear_factor;
	(cur_speed<0.f) ? (cur_speed=-cur_speed) :cur_speed;
	pwheel->ApplyDriveAxisTorque(pwheel->car->RefWheelCurTorque()/gear_factor);
}
void CCar::SWheelDrive::Neutral()
{
	pwheel->ApplyDriveAxisVelTorque(0.f,pwheel->car->m_axle_friction);
}

float CCar::SWheelDrive::ASpeed()
{
	CPhysicsJoint* J=pwheel->joint;
	if(!J) return 0.f;
	return dFabs(dJointGetHinge2Angle2Rate(J->GetDJoint()));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelSteer::Init()
{
	CKinematics* pKinematics=smart_cast<CKinematics*>(pwheel->car->Visual());
	pwheel->Init();
	(bone_map.find(pwheel->bone_id))->second.joint->GetLimits(lo_limit,hi_limit,0);
	CBoneData& bone_data= pKinematics->LL_GetData(u16(pwheel->bone_id));
	switch(bone_data.IK_data.type)
	{
	case jtWheel:	
		pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.i.y;//.dotproduct(pwheel->car->m_root_transform.j);
		break;

	default: NODEFAULT;
	}
	
	pos_right=pos_right>0.f ? -1.f : 1.f;
	float steering_torque=pKinematics->LL_UserData()->r_float("car_definition","steering_torque");
	pwheel->ApplySteerAxisTorque(steering_torque);
	dJointSetHinge2Param(pwheel->joint->GetDJoint(), dParamFudgeFactor, 0.005f/steering_torque);
	pwheel->ApplySteerAxisVel(0.f);
	limited=false;
}

void CCar::SWheelSteer::SteerRight()
{
	limited=true;						//no need to limit wheels when steering
	if(pos_right>0)
	{
		pwheel->SetSteerHiLimit(hi_limit);
		pwheel->ApplySteerAxisVel(pwheel->car->m_steering_speed);
	}
	else
	{
		pwheel->SetSteerLoLimit(lo_limit);
		pwheel->ApplySteerAxisVel( -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerLeft()
{

	limited=true;						//no need to limit wheels when steering
	if(pos_right<0)
	{
		pwheel->SetSteerHiLimit(hi_limit);
		pwheel->ApplySteerAxisVel(pwheel->car->m_steering_speed);
	}
	else
	{
		pwheel->SetSteerLoLimit(lo_limit);
		pwheel->ApplySteerAxisVel( -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerIdle()
{
	limited=false;
	if(pwheel->car->e_state_steer==right)
	{
		if(pos_right<0)
		{
			pwheel->SetSteerHiLimit(0.f);
			pwheel->ApplySteerAxisVel( pwheel->car->m_steering_speed);
		}
		else
		{
			pwheel->SetSteerLoLimit(0.f);
			pwheel->ApplySteerAxisVel(-pwheel->car->m_steering_speed);
		}
	}
	else
	{
		if(pos_right>0)
		{
			pwheel->SetSteerHiLimit(0.f);
			pwheel->ApplySteerAxisVel( pwheel->car->m_steering_speed);
		}
		else
		{
			pwheel->SetSteerLoLimit(0.f);
			pwheel->ApplySteerAxisVel(-pwheel->car->m_steering_speed);
		}
	}

}

void CCar::SWheelSteer::Limit()
{
	CPhysicsJoint* J=pwheel->joint;
	if(!J) return;
	dJointID joint=J->GetDJoint();
	if(!limited)
	{
		dReal angle = dJointGetHinge2Angle1(joint);
		if(dFabs(angle)<M_PI/180.f)
		{
			pwheel->SetSteerLimits(0.f,0.f);
			pwheel->ApplySteerAxisVel(0.f);
			limited=true;
		}
	}
	pwheel->car->b_wheels_limited=pwheel->car->b_wheels_limited&&limited;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelBreak::Init()
{
	pwheel->Init();
	break_torque=pwheel->car->m_break_torque*pwheel->radius/pwheel->car->m_ref_radius;
	hand_break_torque=pwheel->car->m_hand_break_torque*pwheel->radius/pwheel->car->m_ref_radius;
}

void CCar::SWheelBreak::Break()
{
	pwheel->ApplyDriveAxisVelTorque(0.f,100000.f*break_torque);
}

void CCar::SWheelBreak::HandBreak()
{
	pwheel->ApplyDriveAxisVelTorque(0.f,hand_break_torque);
}

void CCar::SWheelBreak::Neutral()
{
	pwheel->ApplyDriveAxisVelTorque(0.f,pwheel->car->m_axle_friction);
}