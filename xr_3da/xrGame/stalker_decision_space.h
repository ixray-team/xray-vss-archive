////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_decision_space.h
//	Created 	: 30.03.2004
//  Modified 	: 30.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker decision space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace StalkerDecisionSpace {
	enum EMotivations {
		eMotivationGlobal			= u32(0),
		eMotivationAlive,
		eMotivationDead,
		eMotivationSolveZonePuzzle,
		eMotivationScript,
		eMotivationDummy			= u32(-1)
	};

	enum EWorldProperties {
		eWorldPropertyAlive			= u32(0),
		eWorldPropertyDead,
		eWorldPropertyAlreadyDead,
		
		eWorldPropertyALife,
		eWorldPropertyPuzzleSolved,

		eWorldPropertyItems,
		eWorldPropertyEnemy,
		
		eWorldPropertySeeEnemy,
		eWorldPropertyItemToKill,
		eWorldPropertyFoundItemToKill,
		eWorldPropertyItemCanKill,
		eWorldPropertyFoundAmmo,
		eWorldPropertyReadyToKill,
		eWorldPropertyKillDistance,
		
		eWorldPropertyScript,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorAlreadyDead	= u32(0),
		eWorldOperatorDead,
		
		eWorldOperatorFreeNoALife,
		eWorldOperatorFreeALife,
		
		eWorldOperatorGatherItems,
		eWorldOperatorKillEnemy,
		eWorldOperatorGetKillDistance,
		eWorldOperatorGetReadyToKill,
		eWorldOperatorGetEnemy,
		eWorldOperatorGetEnemySeen,
		eWorldOperatorGetItemToKill,
		eWorldOperatorFindItemToKill,
		eWorldOperatorMakeItemKilling,
		eWorldOperatorFindAmmo,


		eWorldOperatorScript,
		eWorldOperatorDummy			= u32(-1),
	};
};