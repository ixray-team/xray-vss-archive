#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "ai/monsters/bloodsucker/bloodsucker.h"
#include "ai/monsters/zombie/zombie.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "ai/monsters/monster_home.h"

//////////////////////////////////////////////////////////////////////////
//CAI_Bloodsucker
void CScriptGameObject::set_invisible(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_invisible!");
		return;
	}

	val ? monster->manual_activate() : monster->manual_deactivate();
}

void CScriptGameObject::set_manual_invisibility(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_manual_invisible!");
		return;
	}

	val ? monster->set_manual_switch(true) : monster->set_manual_switch(false);
}
bool CScriptGameObject::get_invisible()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_invisible!");
		return false;
	}

	return monster->CEnergyHolder::is_active();
}

bool CScriptGameObject::get_manual_invisibility()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_manual_invisibility!");
		return false;
	}
	return monster->is_manual_control();
}


CScriptSoundInfo CScriptGameObject::GetSoundInfo()
{
	CScriptSoundInfo	ret_val;

	CBaseMonster *l_tpMonster = smart_cast<CBaseMonster *>(&object());
	if (l_tpMonster) {
		if (l_tpMonster->SoundMemory.IsRememberSound()) {
			SoundElem se; 
			bool bDangerous;
			l_tpMonster->SoundMemory.GetSound(se, bDangerous);

			const CGameObject *pO = smart_cast<const CGameObject *>(se.who);
			ret_val.set((pO && !pO->getDestroy()) ?  pO->lua_game_object() : 0, bDangerous, se.position, se.power, int(se.time));
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetSoundInfo!");
	}
	return			(ret_val);
}

CScriptMonsterHitInfo CScriptGameObject::GetMonsterHitInfo()
{
	CScriptMonsterHitInfo	ret_val;

	CBaseMonster *l_tpMonster = smart_cast<CBaseMonster *>(&object());
	if (l_tpMonster) {
		if (l_tpMonster->HitMemory.is_hit()) {
			CGameObject *pO = smart_cast<CGameObject *>(l_tpMonster->HitMemory.get_last_hit_object());
			ret_val.set((pO && !pO->getDestroy()) ?  pO->lua_game_object() : 0, l_tpMonster->HitMemory.get_last_hit_dir(), l_tpMonster->HitMemory.get_last_hit_time());
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetMonsterHitInfo!");
	}
	return			(ret_val);
}

//////////////////////////////////////////////////////////////////////////
// CBaseMonster
void CScriptGameObject::skip_transfer_enemy(bool val)
{
	CBaseMonster *monster = smart_cast<CBaseMonster *>(&object());
	if (monster) monster->skip_transfer_enemy(val);
}

void CScriptGameObject::set_home(LPCSTR name, float r_min, float r_max)
{
	CBaseMonster *monster = smart_cast<CBaseMonster *>(&object());
	if (monster) monster->Home->setup(name,r_min,r_max);
}
void CScriptGameObject::remove_home()
{
	CBaseMonster *monster = smart_cast<CBaseMonster *>(&object());
	if (monster) monster->Home->remove_home();
}

bool CScriptGameObject::fake_death_fall_down()
{
	CZombie	*monster = smart_cast<CZombie*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CZombie : cannot access class member fake_death_fall_down!");
		return false;
	}

	return monster->fake_death_fall_down();
}
void CScriptGameObject::fake_death_stand_up()
{
	CZombie	*monster = smart_cast<CZombie*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CZombie : cannot access class member fake_death_fall_down!");
		return;
	}

	monster->fake_death_stand_up();
}

