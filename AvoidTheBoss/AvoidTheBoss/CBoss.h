#pragma once
#include "Player.h"
class CBullet;

class CBoss : public CPlayer
{
	friend class CSession;
private:
	CGameObject* m_RightHands = NULL;
private: // 06/11 --> 06/13 �߰�
	int32 m_runAttackAnimTime = 0;
	int32 m_standAttackAnimTime = 0;
	bool  m_IsOnAttack = false ;
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device5* pd3dDevice,  
   
  ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Rotate(float x, float y, float z);
	virtual void PrepareAnimate();
	// ========== �÷��̾� ���� ���� ============
	virtual uint8 ProcessInput(); // 04-29 �߰�
	virtual void Move(const int16& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate(float ,CLIENT_TYPE);
	

	// ============= �ִϸ��̼� Ʈ�� ���� ���� ============ // 05-22 �߰� �Լ�
	void AnimationLogicUpdate();
	void AimationStateUpdate();

	void SetIdleAnimTrack();
	void SetRunAnimTrack();
	void SetAttackAnimTrack();
	void SetRunAttackAnimTrack();
	virtual void AnimTrackUpdate(); 

	void SetOnAttack(bool value) { m_IsOnAttack = value; }
	bool GetOnAttack() { return m_IsOnAttack; }
	void SetAttackAnimTime()
	{
		m_standAttackAnimTime = 0;
	}
	void SetRunAttackAnimTime()
	{
		m_runAttackAnimTime = 0;
	}

	void SetAttackAnimOtherClient();
};