#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnSwitchInteration = false; // F키를 눌렀다 땠는지 확인하는 용도
private:
	bool m_bIsInSwitchArea = false;
public:
	CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType);
	virtual ~CEmployee();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	

	// ========== 플레이어 조작 관련 ===================
	virtual void ProcessInput(const int16&);
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype);
	// ============= 애니메이션 트랙 셋팅 관련 ============
	void SetIdleAnimTrack(); // 걷기
	void SetRunAnimTrack(); // 달리기

	void SetAttackedAnimTrack(); // 절뚝거리기 
	void SetDownAnimTrack(); // 피격
	void SetCrawlAnimTrack(); // 기어가기
	void SetStandAnimTrack(); // 일어나기
	void SetInteractionAnimTrack(); // 발전기 상호작용

	virtual void AnimTrackUpdate();

	void SetInteractionAnimTrackOtherClient();
	
	bool IsPlayerCanSwitchInteraction() { return m_bIsInSwitchArea; }
	int32 GetAvailableSwitchIdx();

	SwitchInformation m_pSwitches[3];
};