#pragma once

class ENGINE_API CObject;

namespace Feel
{
	class	ENGINE_API Sound
	{
	public:
		virtual		void			feel_sound_new		(CObject* who, int type, CSoundUserDataPtr user_data, const Fvector& Position, float power) {};
	};
};
