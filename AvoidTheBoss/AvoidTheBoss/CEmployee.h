#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnSwitchInteration = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
private:
	bool m_bIsInGenArea = false;
	bool m_bIsInDownPlayerArea = false; // Down�� �÷��̾�� ������ �ִ°�?
public:
	int32 m_attackedAnimationCount = 0;
	int32 m_downAnimationCount = 0;
	int32 m_standAnimationCount = 0;
public:
	CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType);
	virtual ~CEmployee();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	

	// ========== �÷��̾� ���� ���� ===================
	virtual void ProcessInput(const int16&);
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype);
	// ============= �ִϸ��̼� Ʈ�� ���� ���� ============
	void SetIdleAnimTrack(); // �ȱ�
	void SetRunAnimTrack(); // �޸���

	void SetAttackedAnimTrack(); // ���ҰŸ��� 
	void SetDownAnimTrack(); // �ǰ�
	void SetCrawlAnimTrack(); // ����
	void SetStandAnimTrack(); // �Ͼ��
	void SetInteractionAnimTrack(); // ������ ��ȣ�ۿ�

	virtual void AnimTrackUpdate();
	// ================ �ٸ� Ŭ���̾�Ʈ �ִϸ��̼� ��� ����
	void SetInteractionAnimTrackOtherClient();

	// ================ ĳ���� ���� ��ȯ ============ 05-23 �߰��Լ�
public: // 05-23 �߰� �Լ�
	void PlayerAttacked();
	void PlayerDown();
	int32 GetPlayerBehavior() { return m_behavior; }
	
	bool GetIsInGenArea() { return m_bIsInGenArea; }
	int32 GetAvailGenIdx();
	int32 GetAvailEMPldx();
public: // 05-24 �߰��Լ�
	SwitchInformation m_pSwitches[3];
};