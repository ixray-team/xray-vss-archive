////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_anomaly_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker anomaly action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetOutOfAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetOutOfAnomaly : public CStalkerActionBase {
private:
	string512			m_temp;

protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetOutOfAnomaly	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetectAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDetectAnomaly : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionDetectAnomaly	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

