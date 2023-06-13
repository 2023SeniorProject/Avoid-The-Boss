#pragma once
#include "Player.h"
class CBullet;

class CBoss : public CPlayer
{
	friend class CSession;
private:
	CGameObject* m_RightHands = NULL;
private: // 06/11 --> 06/13 추가
	int32 m_runAttackAnimTime = BOSS_RUNATTACK_TIME;
	int32 m_standAttackAnimTime = BOSS_ATTACK_TIME;
	bool  m_IsOnAttack;
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Rotate(float x, float y, float z);
	virtual void PrepareAnimate();
	// ========== 플레이어 조작 관련 ============
	virtual uint8 ProcessInput(); // 04-29 추가
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float ,CLIENT_TYPE);
	

	// ============= 애니메이션 트랙 셋팅 관련 ============ // 05-22 추가 함수
	void SetIdleAnimTrack();
	void SetRunAnimTrack();
	void SetAttackAnimTrack();
	void SetRunAttackAnimTrack();
	virtual void AnimTrackUpdate(); 

	void SetOnAttack(bool value) { m_IsOnAttack = value; }
	bool GetOnAttack() { return m_IsOnAttack; }
	void SetAttackAnimTime()
	{
		m_standAttackAnimTime = BOSS_ATTACK_TIME;
	}
	void SetRunAttackAnimTime()
	{
		m_runAttackAnimTime = BOSS_RUNATTACK_TIME;
	}

	void SetAttackAnimOtherClient();
	
};