#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnSwitchInteration = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
private:
	bool m_bIsInSwitchArea = false;
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
	void SetInteractionAnimTrack();
	void SetIdleAnimTrack();
	void SetRunAnimTrack();
	void SetAttackedAnimTrack() {};
	void SetDownAnimTrack() {};
	virtual void AnimTrackUpdate() {};

	void SetInteractionAnimTrackOtherClient();
	
	bool IsPlayerCanSwitchInteraction() { return m_bIsInSwitchArea; }
	int32 GetAvailableSwitchIdx();

	SwitchInformation m_pSwitches[3];
};