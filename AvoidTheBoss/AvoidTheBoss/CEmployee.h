#pragma once
#include "Player.h"


class CGenerator;

class CEmployee : public CPlayer
{
	friend class UIManager;
public:
	bool m_bIsPlayerOnGenInter = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
	bool m_bIsPlayerOnRescueInter = false;
public:
	bool m_bIsInvincibility = false;
	float m_UICoolTime = 1.0f;
	
private:
	bool m_bIsInGenArea = false;
	bool m_bIsInDownPlayerArea = false; // Down�� �÷��̾�� ������ �ִ°�?
	//bool m_bIsDown
protected:
	float m_maxRGuage = 200;
	float m_curGuage = 0;
	float m_rVel = 10.0f;
	float m_bIsRescuing = false;
private:
	int32 m_curInterGen = -1;
public:
	int32 m_deadCnt = 0;
	int32 m_activeCnt = 0;
	
	int32 m_attackedAnimationCount = 0;
	int32 m_downAnimationCount = 0;
	int32 m_standAnimationCount = 0;
public:
	CEmployee(ID3D12Device5* pd3dDevice, 
		ID3D12GraphicsCommandList4
		* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType);
	virtual ~CEmployee();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	

	// ========== �÷��̾� ���� ���� ===================
	virtual uint8 ProcessInput();
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype);

	void SetGenInteraction(bool value) { m_bIsPlayerOnGenInter = value; }
	bool GetIsPlayerOnGenInter() { return m_bIsPlayerOnGenInter; }
	void SetRescueInteraction(bool value) { m_bIsPlayerOnRescueInter = value; }
	bool GetIsPlayerOnRescueInter() { return m_bIsPlayerOnRescueInter; }
	
	// ============= �ִϸ��̼� Ʈ�� ���� ���� ============

	bool IsMovable() 
	{ 
		return (m_behavior == (int32)PLAYER_BEHAVIOR::RESCUE || m_behavior == (int32)PLAYER_BEHAVIOR::SWITCH_INTER || m_behavior == (int32)PLAYER_BEHAVIOR::CRAWL 
			|| m_behavior == (int32)PLAYER_BEHAVIOR::EXIT);
	}
	bool IsSeMiBehavior() // ���ĵ�, ũ���, �ٿ� ����
	{
		return (m_behavior == (int32)PLAYER_BEHAVIOR::DOWN  || m_behavior == (int32)PLAYER_BEHAVIOR::STAND);
	}
	
	// �����
	void RescueOn(bool value) 
	{ 
		if(m_bIsRescuing != value ) m_bIsRescuing = value;
		
	}
	void ResetRescueGuage() { m_curGuage = 0; }
	bool GetRescueOn() { return m_bIsRescuing; }

	virtual void ResetState()
	{
		SetBehavior(PLAYER_BEHAVIOR::IDLE);
		m_attackedAnimationCount = 0;
		m_downAnimationCount = 0;
		m_standAnimationCount = 0;

		m_bIsPlayerOnGenInter = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
		m_bIsPlayerOnRescueInter = false;
	
		m_bIsInvincibility = false;
		m_UICoolTime = 1.0f;

	
		m_bIsInGenArea = false;
		m_bIsInDownPlayerArea = false; // Down�� �÷��̾�� ������ �ִ°�?

	
	    m_curGuage = 0;

		m_bIsRescuing = false;

	}

	// �Ѿ� �°� ������ x,2
	// �ǰ� 2,4
	// ������ �ȱ� 2,4

	//down (�Ѿ� �°� ������) x 
	//down_idle (������ ����) ��
	//slow_walk,crawl (���ҰŸ���) x

	void SetIdleAnimTrack();	// �ȱ� 0
	void SetRunAnimTrack(); 	// �޸��� 1
	void SetDownAnimTrack();	// �Ѿ� �°� ������ x,2
	void SetAttackedAnimTrack();// ���ҰŸ��� 2,4
	void SetCrawlAnimTrack();	// ������ ���� x,3
	void SetStandAnimTrack(); 	// �Ͼ�� x,5
	void SetInteractionAnimTrack(); 	// ������ ��ȣ�ۿ� 3,6
	void SetExitMotionAnimTrack();

	virtual void AnimTrackUpdate();


	// ================ ĳ���� ���� ��ȯ ============ 05-23 �߰��Լ�
public: // 05-23 �߰� �Լ�
	void PlayerAttacked();
	void PlayerDown();
	bool GenTasking();
	bool RescueTasking();
	
	
	bool GetIsInGenArea() { return m_bIsInGenArea; }
	CGenerator* GetAvailGen();
	CEmployee* GetAvailEMP();
public: // 05-24 �߰��Լ�
	GEN_INFO m_pSwitches[3];
};