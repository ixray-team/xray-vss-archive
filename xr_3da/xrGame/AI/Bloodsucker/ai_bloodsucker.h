#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Bloodsucker : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void	Init					();

	virtual void	Think					();
	virtual void	UpdateCL				();

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);

	// Flesh-specific FSM
	CBitingRest			*stateRest;
	CBitingAttack		*stateAttack;
	CBitingEat			*stateEat;
	CBitingHide			*stateHide;
	CBitingDetour		*stateDetour;
	CBitingPanic		*statePanic;
	CBitingExploreDNE	*stateExploreDNE;
	CBitingExploreDE	*stateExploreDE;
	CBitingExploreNDE	*stateExploreNDE;

	friend	class		CBitingRest;
	friend  class 		CBitingAttack;
	friend	class 		CBitingEat;
	friend	class 		CBitingHide;
	friend	class 		CBitingDetour;
	friend	class 		CBitingPanic;
	friend	class 		CBitingExploreDNE;
	friend	class 		CBitingExploreDE;
	friend	class 		CBitingExploreNDE;

};

