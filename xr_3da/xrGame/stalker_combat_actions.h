////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetKillDistance
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetKillDistance : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetKillDistance	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetEnemy : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
								CStalkerActionGetEnemy	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void				initialize				();
	virtual void				execute					();
	virtual void				finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemySeen
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetEnemySeen : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetEnemySeen	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetItemToKill : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetItemToKill	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFindItemToKill : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionFindItemToKill(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

class CStalkerActionMakeItemKilling : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionMakeItemKilling	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindAmmo
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFindAmmo : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
								CStalkerActionFindAmmo	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void				initialize				();
	virtual void				execute					();
	virtual void				finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillVeryAggressive
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKillVeryAggressive : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetReadyToKillVeryAggressive(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyVeryAggressive
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyVeryAggressive : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionKillEnemyVeryAggressive(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillAggressive
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKillAggressive : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetReadyToKillAggressive(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyAggressive
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyAggressive : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	bool				m_fire_crouch;

public:
						CStalkerActionKillEnemyAggressive(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillModerate
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKillModerate : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetReadyToKillModerate(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyModerate
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyModerate : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	bool				m_fire_crouch;

public:
						CStalkerActionKillEnemyModerate(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillAvoid
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKillAvoid : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetReadyToKillAvoid(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyAvoid
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyAvoid : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionKillEnemyAvoid(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionRetreatFromEnemy : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionRetreatFromEnemy(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionAimEnemy : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	bool				m_run;

public:
						CStalkerActionAimEnemy		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCamp
//////////////////////////////////////////////////////////////////////////

class CStalkerActionCamp : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionCamp			(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemySeenModerate
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetEnemySeenModerate : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	u32					m_start_standing_time;

public:
						CStalkerActionGetEnemySeenModerate			(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyLostModerate
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyLostModerate : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionKillEnemyLostModerate(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionTakeCover : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionTakeCover		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

