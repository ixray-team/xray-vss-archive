////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_sounds.h
//	Created 	: 15.08.2002
//  Modified 	: 15.08.2002
//	Author		: Dmitriy Iassenev
//	Description : Sounds for AI personalities
////////////////////////////////////////////////////////////////////////////

#ifndef ai_soundsH
#define ai_soundsH

enum ESoundTypes {
	SOUND_TYPE_NO_SOUND					= 0x00000000ui32,

	SOUND_TYPE_WEAPON					= 0x80000000ui32,
	SOUND_TYPE_ITEM						= 0x40000000ui32,
	SOUND_TYPE_MONSTER					= 0x20000000ui32,
	SOUND_TYPE_ANOMALY					= 0x10000000ui32,
	SOUND_TYPE_WORLD					= 0x08000000ui32,

	SOUND_TYPE_PICKING_UP				= 0x04000000ui32,
	SOUND_TYPE_DROPPING					= 0x02000000ui32,
	SOUND_TYPE_HIDING					= 0x01000000ui32,
	SOUND_TYPE_TAKING					= 0x00800000ui32,
	SOUND_TYPE_USING					= 0x00400000ui32,

	SOUND_TYPE_SHOOTING					= 0x00200000ui32,
	SOUND_TYPE_EMPTY_CLICKING			= 0x00100000ui32,
	SOUND_TYPE_BULLET_HIT				= 0x00080000ui32,
	SOUND_TYPE_RECHARGING				= 0x00040000ui32,

	SOUND_TYPE_DYING					= 0x00020000ui32,
	SOUND_TYPE_INJURING					= 0x00010000ui32,
	SOUND_TYPE_STEP						= 0x00008000ui32,
	SOUND_TYPE_TALKING					= 0x00004000ui32,
	SOUND_TYPE_ATTACKING				= 0x00002000ui32,
	SOUND_TYPE_EATING					= 0x00001000ui32,

	SOUND_TYPE_IDLE						= 0x00000800ui32,

	SOUND_TYPE_OBJECT_BREAKING			= 0x00000400ui32,
	SOUND_TYPE_OBJECT_COLLIDING			= 0x00000200ui32,
	SOUND_TYPE_OBJECT_EXPLODING			= 0x00000100ui32,
	SOUND_TYPE_AMBIENT					= 0x00000080ui32,
	
	SOUND_TYPE_ITEM_PICKING_UP			= SOUND_TYPE_ITEM | SOUND_TYPE_PICKING_UP,
	SOUND_TYPE_ITEM_DROPPING			= SOUND_TYPE_ITEM | SOUND_TYPE_DROPPING,
	SOUND_TYPE_ITEM_HIDING				= SOUND_TYPE_ITEM | SOUND_TYPE_HIDING,
	SOUND_TYPE_ITEM_TAKING				= SOUND_TYPE_ITEM | SOUND_TYPE_TAKING,
	SOUND_TYPE_ITEM_USING				= SOUND_TYPE_ITEM | SOUND_TYPE_USING,

	SOUND_TYPE_WEAPON_SHOOTING			= SOUND_TYPE_WEAPON | SOUND_TYPE_SHOOTING,
	SOUND_TYPE_WEAPON_EMPTY_CLICKING	= SOUND_TYPE_WEAPON | SOUND_TYPE_EMPTY_CLICKING,
	SOUND_TYPE_WEAPON_BULLET_HIT		= SOUND_TYPE_WEAPON | SOUND_TYPE_BULLET_HIT,
	SOUND_TYPE_WEAPON_RECHARGING		= SOUND_TYPE_WEAPON | SOUND_TYPE_RECHARGING,

	SOUND_TYPE_MONSTER_DYING			= SOUND_TYPE_MONSTER | SOUND_TYPE_DYING,
	SOUND_TYPE_MONSTER_INJURING			= SOUND_TYPE_MONSTER | SOUND_TYPE_INJURING,
	SOUND_TYPE_MONSTER_STEP				= SOUND_TYPE_MONSTER | SOUND_TYPE_STEP,
	SOUND_TYPE_MONSTER_TALKING			= SOUND_TYPE_MONSTER | SOUND_TYPE_TALKING,
	SOUND_TYPE_MONSTER_ATTACKING		= SOUND_TYPE_MONSTER | SOUND_TYPE_ATTACKING,
	SOUND_TYPE_MONSTER_EATING			= SOUND_TYPE_MONSTER | SOUND_TYPE_EATING,
	
	SOUND_TYPE_ANOMALY_IDLE				= SOUND_TYPE_ANOMALY | SOUND_TYPE_IDLE,

	SOUND_TYPE_WORLD_OBJECT_BREAKING	= SOUND_TYPE_WORLD | SOUND_TYPE_OBJECT_BREAKING,
	SOUND_TYPE_WORLD_OBJECT_COLLIDING	= SOUND_TYPE_WORLD | SOUND_TYPE_OBJECT_COLLIDING,
	SOUND_TYPE_WORLD_OBJECT_EXPLODING	= SOUND_TYPE_WORLD | SOUND_TYPE_OBJECT_EXPLODING,
	SOUND_TYPE_WORLD_AMBIENT			= SOUND_TYPE_WORLD | SOUND_TYPE_AMBIENT,

	SOUND_TYPE_WEAPON_PISTOL			= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_GUN				= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_SUBMACHINEGUN		= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_MACHINEGUN		= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_SNIPERRIFLE		= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_GRENADELAUNCHER	= SOUND_TYPE_WEAPON,
	SOUND_TYPE_WEAPON_ROCKETLAUNCHER	= SOUND_TYPE_WEAPON,
};

#define CROUCH_SOUND_FACTOR					.3f
#define ACCELERATED_SOUND_FACTOR			.5f

extern xr_token anomaly_type_token[];

extern float ffGetStartVolume(ESoundTypes eType);

#endif
