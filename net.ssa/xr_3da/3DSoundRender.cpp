// 3DSoundRender.cpp: implementation of the CSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundRender::CSoundRender()
{
}

CSoundRender::~CSoundRender()
{
}

void CSoundRender::OnMove()
{
}

u32 CSoundRender::FindByName			(LPCSTR name, BOOL _3D, BOOL _Freq) {
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			if ((stricmp(sounds[i][0]->fName,name)==0)&&(_3D==sounds[i][0]->_3D)&&(_Freq==sounds[i][0]->_Freq))  
				return i;
		}
	}
	return soundUndefinedHandle;
}

u32 CSoundRender::FindEmptySlot		()
{
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()==0) return i;
	}
	return soundUndefinedHandle;
}

u32	CSoundRender::Append				(CSound *p)
{
	// empty slot not found - expand lists
	vector <CSound *>	pv;
	sounds.push_back( pv );
	refcounts.push_back(1);
	int i = sounds.size()-1;
	sounds[i].push_back(p);
	return i;
}

u32	CSoundRender::CreateSound			(LPCSTR name, BOOL _3D, BOOL _Freq, BOOL bNotClip)
{
	int fnd;
	if ((fnd=FindByName(name,_3D, _Freq))>=0) {
		refcounts[fnd]	+=1;
		return fnd;
	}

	// sound not found - create _new_
	CSound *pSnd	= xr_new<CSound> (_3D);
	pSnd->Load		(name,_Freq);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

void CSoundRender::DeleteSound		(u32& hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	VERIFY(refcounts[hSound]>0);
	refcounts[hSound]-=1;
	if (refcounts[hSound]==0) {
		// all references destroyed - destroy sound as itself
		for (u32 i=0; i<sounds[hSound].size(); i++)
		{
			xr_delete(sounds[hSound][i]);
		}
		sounds[hSound].clear();
	}
	hSound = soundUndefinedHandle;
}

CSound* CSoundRender::GetFreeSound	(u32 hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	for (u32 i=0; i<sounds[hSound].size(); i++)
	{
		if (!sounds[hSound][i]->isPlaying())
			return sounds[hSound][i];
	}
	// _free sound not found - create duplicate
	CSound *pSnd = xr_new<CSound>	(sounds[hSound].front()->_3D);
	pSnd->Load						(sounds[hSound].front());
	sounds[hSound].push_back		(pSnd);
	return pSnd;
}

void CSoundRender::Play		(u32 hSound, sound* P, BOOL bLoop, int iLoopCnt)
{
	P->feedback				= GetFreeSound	(hSound);
	P->feedback->Play		(P, bLoop, iLoopCnt);
}

void CSoundRender::Reload	()
{
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			// Main sound
			sounds[i][0]->Stop();
			_RELEASE(sounds[i][0]->pBuffer3D);
			_RELEASE(sounds[i][0]->pBuffer);
			sounds[i][0]->Load(LPSTR(0));
			sounds[i][0]->bNeedUpdate = true;
			if (sounds[i][0]->isPlaying()) {
				sounds[i][0]->Play(sounds[i][0]->owner,sounds[i][0]->bMustLoop, sounds[i][0]->iLoopCount);
			}

			// Clones
			for (u32 j=1; j<sounds[i].size(); j++) {
				sounds[i][j]->Stop();
				_RELEASE(sounds[i][j]->pBuffer3D);
				_RELEASE(sounds[i][j]->pBuffer);
				SoundRender.pDevice->DuplicateSoundBuffer(sounds[i][0]->pBuffer,&sounds[i][j]->pBuffer);
				VERIFY	(sounds[i][j]->pBuffer);
				sounds[i][j]->pBuffer->QueryInterface(IID_IDirectSound3DBuffer8,(void **)(&sounds[i][j]->pBuffer3D));
				sounds[i][j]->bNeedUpdate = true;
				if (sounds[i][j]->isPlaying()) {
					sounds[i][j]->Play(sounds[i][j]->owner,sounds[i][j]->bMustLoop, sounds[i][j]->iLoopCount);
				}
			}
		}
	}
}
