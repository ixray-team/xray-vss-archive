// EntityCondition.h: ����� ��������� ����� ��������
//
//////////////////////////////////////////////////////////////////////
 
#ifndef _ENTITY_CONDITION_H_
#define _ENTITY_CONDITION_H_
#pragma once


#include "gameobject.h"

class CEntityCondition
{
public:
	CEntityCondition(void);
	virtual ~CEntityCondition(void);

	virtual void Load(LPCSTR section);

	float GetHealth() const {return m_fHealth;}
	float GetPower() const {return m_fPower;}
	float GetSatiety() const {return m_fSatiety;}
	float GetRadiation() const {return m_fRadiation;}

	float GetCircumspection() const {return m_fCircumspection;}
	float GetEntityMorale() const {return m_fEntityMorale;}

	float GetMaxHealth() const {return m_fHealthMax;}
	float GetHealthLost() const {return m_fHealthLost;}

	virtual bool IsLimping();

	void ChangeHealth(float value);
	void ChangePower(float value);
	void ChangeSatiety(float value);
	void ChangeRadiation(float value);

	void ChangeBleeding(float percent);

	void ChangeCircumspection(float value);
	void ChangeEntityMorale(float value);

	//hit_power �������� �� 0 �� 100 (��������� �����������)
	virtual void ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	//���������� ��������� � �������� �������
	virtual void UpdateCondition();

	//��������! ��� �����
	virtual void Sleep(float hours);
	
	virtual void GoSleep();
	virtual void Awoke();
	
	//�������� ������ ����� �� ���� �������� ��� 
	virtual float BleedingSpeed();

	virtual CObject* GetWhoHitLastTime() {return m_pWho;}

protected:
	virtual void UpdateHealth();
	virtual void UpdatePower();
	virtual void UpdateSatiety();
	virtual void UpdateRadiation();

	virtual void UpdateCircumspection();
	virtual void UpdateEntityMorale();

	virtual void AddWound(float hit_power, s16 element = 0);

	//��������� ���� ���� � ����������� �� �������� �������
	//(������ ��� InventoryOwner)
	virtual float HitOutfitEffect(float hit_power, ALife::EHitType hit_type);
	


	//��� �������� ��������� �������� ���,
	//������������ ����� ���� ��� ������� ���
	//� �������� ������ ����� �� ����
	DEFINE_MAP (s16, float, WOUND_MAP, WOUND_PAIR_IT);
	WOUND_MAP m_mWound;

	//��� �������� �� 0 �� 1			
	float m_fHealth;				//��������
	float m_fPower;					//����
	float m_fSatiety;				//������� (�������)
	float m_fRadiation;				//���� ������������� ���������

	float m_fCircumspection;		//���������������	
	float m_fEntityMorale;			//������

	//������������ ��������
	float m_fHealthMax;				
	float m_fPowerMax;					
	float m_fSatietyMax;				
	float m_fRadiationMax;				

	float m_fCircumspectionMax;		
	float m_fEntityMoraleMax;				

	//�������� ��������� ���������� �� ������ ����������
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaSatiety;
	float m_fDeltaRadiation;

	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	//�������� ��������� ������������� ���������
	//� �������
	float m_fV_Health;
	float m_fV_Power;				
	float m_fV_Satiety;	
	float m_fV_Radiation;

	float m_fV_Circumspection;		
	float m_fV_EntityMorale;

	//������������ ��������� ��������� ���������� �� ����� ���
	float m_fK_SleepHealth;
	float m_fK_SleepPower;				
	float m_fK_SleepSatiety;	
	float m_fK_SleepRadiation;

	//������� ���������
	float m_fCurrentSleepHealth;
	float m_fCurrentSleepPower;
	float m_fCurrentSleepSatiety;
	float m_fCurrentSleepRadiation;


	//�������� �������������� �������� � ����
	//�� ���� �������
	float m_fV_SatietyPower;
	float m_fV_SatietyHealth;

	//����������� �������� ������� (� ��������� 0..1), ����� �������� 
	//�������� ����������� ��������
	float m_fSatietyCritical;

	//����������� ��������� �������� �������� �������� �� ��������
	//(��� ������������ ���� ���������)
	float m_fV_RadiationHealth;

	//�������� ������ ����� � �������� ���� �� ������������� ���� 
	float m_fV_Bleeding;
	//�������� ��������� ����
	float m_fV_WoundIncarnation;


	//����� ����, ������������� �� ���������� �������� � ����
	float m_fHealthHitPart;
	float m_fPowerHitPart;

	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//(��� �������� �������� � ����������� ��������)
	float m_fK_Burn;
	float m_fK_Strike;
	float m_fK_Shock;
	float m_fK_Wound;
	float m_fK_Radiation;
	float m_fK_Telepatic;
	float m_fK_ChemicalBurn;
	float m_fK_Explosion;
	float m_fK_FireWound;


	//��������� ���
	bool m_bIsSleeping;

	//������ �������� �� ���������� ����
	float m_fHealthLost;
	//��� ����� ��������� ���
	CObject* m_pWho;


	//��� ������������ ������� 
	u64 m_iLastTimeCalled;
	u64 m_iDeltaTime;
	bool m_bTimeValid;
};

#endif //_ENTITY_CONDITION_H_