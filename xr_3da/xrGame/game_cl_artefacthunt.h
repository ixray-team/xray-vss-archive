#pragma once
#include "game_cl_teamdeathmatch.h"

class game_cl_ArtefactHunt :public game_cl_TeamDeathmatch
{
	typedef game_cl_TeamDeathmatch inherited;
protected:
	ref_sound						pMessageSounds[8];

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);

public :
										game_cl_ArtefactHunt	();
		virtual							~game_cl_ArtefactHunt	();
		virtual		LPCSTR				type_name				() const {return "artefacthunt";};
					void				net_import_state		(NET_Packet& P);
};
