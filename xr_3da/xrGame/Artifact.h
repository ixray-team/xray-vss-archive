#pragma once
#include "inventory.h"

class CArtifact : public CInventoryItem
{
private:
	typedef	CInventoryItem	inherited;
public:
	CArtifact(void);
	virtual ~CArtifact(void);

	virtual void Load				(LPCSTR section);
	
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();
	
	virtual void UpdateCL			();
	virtual void shedule_Update		(u32 dt);	
	
	virtual void renderable_Render	();
	

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	float GetDetectionDist() {return m_fDetectionDist;}

protected:
	//���������� ����������� ��������� ����������
	float		m_fDetectionDist;
	
	//�����
	ref_str		m_detectorSoundName;
	ref_sound	m_detectorSound;
	
	static xr_set<CArtifact*> m_all;
};
