#pragma once
#include "Player.h"


class CGenerator;

class CEmployee : public CPlayer
{
	friend class UIManager;
public:
	bool m_bIsPlayerOnGenInter = false; // F키를 눌렀다 땠는지 확인하는 용도
	bool m_bIsPlayerOnRescueInter = false;
public:
	bool m_bIsInvincibility = false;
	float m_UICoolTime = 1.0f;
	
private:
	bool m_bIsInGenArea = false;
	bool m_bIsInDownPlayerArea = false; // Down된 플레이어와 인접해 있는가?
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
	

	// ========== 플레이어 조작 관련 ===================
	virtual uint8 ProcessInput();
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype);

	void SetGenInteraction(bool value) { m_bIsPlayerOnGenInter = value; }
	bool GetIsPlayerOnGenInter() { return m_bIsPlayerOnGenInter; }
	void SetRescueInteraction(bool value) { m_bIsPlayerOnRescueInter = value; }
	bool GetIsPlayerOnRescueInter() { return m_bIsPlayerOnRescueInter; }
	
	// ============= 애니메이션 트랙 셋팅 관련 ============

	bool IsMovable() 
	{ 
		return (m_behavior == (int32)PLAYER_BEHAVIOR::RESCUE || m_behavior == (int32)PLAYER_BEHAVIOR::SWITCH_INTER || m_behavior == (int32)PLAYER_BEHAVIOR::CRAWL 
			|| m_behavior == (int32)PLAYER_BEHAVIOR::EXIT);
	}
	bool IsSeMiBehavior() // 스탠드, 크라울, 다운 상태
	{
		return (m_behavior == (int32)PLAYER_BEHAVIOR::DOWN  || m_behavior == (int32)PLAYER_BEHAVIOR::STAND);
	}
	
	// 깨우기
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

		m_bIsPlayerOnGenInter = false; // F키를 눌렀다 땠는지 확인하는 용도
		m_bIsPlayerOnRescueInter = false;
	
		m_bIsInvincibility = false;
		m_UICoolTime = 1.0f;

	
		m_bIsInGenArea = false;
		m_bIsInDownPlayerArea = false; // Down된 플레이어와 인접해 있는가?

	
	    m_curGuage = 0;

		m_bIsRescuing = false;

	}

	// 총알 맞고 쓰러짐 x,2
	// 피격 2,4
	// 느리게 걷기 2,4

	//down (총알 맞고 쓰러짐) x 
	//down_idle (쓰러진 상태) ㅇ
	//slow_walk,crawl (절뚝거리기) x

	void SetIdleAnimTrack();	// 걷기 0
	void SetRunAnimTrack(); 	// 달리기 1
	void SetDownAnimTrack();	// 총알 맞고 쓰러짐 x,2
	void SetAttackedAnimTrack();// 절뚝거리기 2,4
	void SetCrawlAnimTrack();	// 쓰러진 상태 x,3
	void SetStandAnimTrack(); 	// 일어나기 x,5
	void SetInteractionAnimTrack(); 	// 발전기 상호작용 3,6
	void SetExitMotionAnimTrack();

	virtual void AnimTrackUpdate();


	// ================ 캐릭터 상태 반환 ============ 05-23 추가함수
public: // 05-23 추가 함수
	void PlayerAttacked();
	void PlayerDown();
	bool GenTasking();
	bool RescueTasking();
	
	
	bool GetIsInGenArea() { return m_bIsInGenArea; }
	CGenerator* GetAvailGen();
	CEmployee* GetAvailEMP();
public: // 05-24 추가함수
	GEN_INFO m_pSwitches[3];
};