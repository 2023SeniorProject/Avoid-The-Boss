#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnSwitchInteration = false; // F키를 눌렀다 땠는지 확인하는 용도
private:
	bool m_bIsInGenArea = false;
	bool m_bIsInDownPlayerArea = false; // Down된 플레이어와 인접해 있는가?
private:
	float m_maxRGuage = 100;
	float m_curGuage = 0;
	float m_rVel = 5.0f;
	float m_bIsRescuing = false;
public:
	int32 m_attackedAnimationCount = 0;
	int32 m_downAnimationCount = 0;
	int32 m_standAnimationCount = 0;
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
	bool IsMovable() 
	{ 
		return !(m_behavior == (int32)PLAYER_BEHAVIOR::DOWN || 
			m_behavior == (int32)PLAYER_BEHAVIOR::CRAWL || m_behavior == (int32)PLAYER_BEHAVIOR::STAND 
			|| m_behavior == (int32)PLAYER_BEHAVIOR::RESCUE || m_behavior == (int32)PLAYER_BEHAVIOR::SWITCH_INTER);
	}
	
	// 깨우기
	void RescueOn(bool value) 
	{ 
		if (m_behavior == (int32)PLAYER_BEHAVIOR::CRAWL) m_bIsRescuing = true;

	}
	void ResetRescueGuage() { m_curGuage = 0; }
	bool GetRescueOn() { return m_bIsRescuing; }

	void SetAttackedAnimTrack(); // 절뚝거리기 
	void SetDownAnimTrack(); // 피격
	void SetCrawlAnimTrack(); // 기어가기
	void SetStandAnimTrack(); // 일어나기
	void SetInteractionAnimTrack(); // 발전기 상호작용
	

	virtual void AnimTrackUpdate();
	// ================ 다른 클라이언트 애니메이션 재생 전용
	void SetInteractionAnimTrackOtherClient();

	// ================ 캐릭터 상태 반환 ============ 05-23 추가함수
public: // 05-23 추가 함수
	void PlayerAttacked();
	void PlayerDown();
	int32 GetPlayerBehavior() { return m_behavior; }
	
	bool GetIsInGenArea() { return m_bIsInGenArea; }
	int32 GetAvailGenIdx();
	int32 GetAvailEMPldx();
public: // 05-24 추가함수
	SwitchInformation m_pSwitches[3];
};