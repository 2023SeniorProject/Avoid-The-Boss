#pragma once
#include "Player.h"

class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnGenInter = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
	bool m_bIsPlayerOnRescueInter = false;
private:
	bool m_bIsInGenArea = false;
	bool m_bIsInDownPlayerArea = false; // Down�� �÷��̾�� ������ �ִ°�?
public:
	// �ǰ� ����Ʈ �ڵ�
	//CHitEffect* m_pHitEffect = NULL;
	//bool m_bIsAttacked = false;
private:
	float m_maxRGuage = 100;
	float m_curGuage = 0;
	float m_rVel = 5.0f;
	float m_bIsRescuing = false;
private:
	int32 m_curInterGen = -1;
public:
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
	virtual void Move(const int8& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype);

	void SetGenInteraction(bool value) { m_bIsPlayerOnGenInter = value; }
	bool GetIsPlayerOnGenInter() { return m_bIsPlayerOnGenInter; }
	void SetRescueInteraction(bool value) { m_bIsPlayerOnRescueInter = value; }
	bool GetIsPlayerOnRescueInter() { return m_bIsPlayerOnRescueInter; }

	// ============= �ִϸ��̼� Ʈ�� ���� ���� ============
	void SetIdleAnimTrack(); // �ȱ�
	void SetRunAnimTrack(); // �޸���
	bool IsMovable() 
	{ 
		return !(m_behavior == (int32)PLAYER_BEHAVIOR::RESCUE || m_behavior == (int32)PLAYER_BEHAVIOR::SWITCH_INTER);
	}
	bool IsSeMiBehavior() // ���ĵ�, ũ���, �ٿ� ����
	{
		return !(m_behavior == (int32)PLAYER_BEHAVIOR::DOWN || m_behavior == (int32)PLAYER_BEHAVIOR::CRAWL || m_behavior == (int32)PLAYER_BEHAVIOR::STAND);
	}
	
	// �����
	void RescueOn(bool value) 
	{ 
		if(m_bIsRescuing != value ) m_bIsRescuing = value;
		
	}
	void ResetRescueGuage() { m_curGuage = 0; }
	bool GetRescueOn() { return m_bIsRescuing; }

	void SetAttackedAnimTrack(); // ���ҰŸ��� 
	void SetDownAnimTrack(); // �ǰ�
	void SetCrawlAnimTrack(); // ����
	void SetStandAnimTrack(); // �Ͼ��
	void SetInteractionAnimTrack(); // ������ ��ȣ�ۿ�
	

	virtual void AnimTrackUpdate();


	// ================ ĳ���� ���� ��ȯ ============ 05-23 �߰��Լ�
public: // 05-23 �߰� �Լ�
	void PlayerAttacked();
	void PlayerDown();
	bool GenTasking();
	bool RescueTasking();
	
	
	bool GetIsInGenArea() { return m_bIsInGenArea; }
	int32 GetAvailGenIdx();
	int32 GetAvailEMPldx();

	
public: // 05-24 �߰��Լ�
	GEN_INFO m_pSwitches[3];
};