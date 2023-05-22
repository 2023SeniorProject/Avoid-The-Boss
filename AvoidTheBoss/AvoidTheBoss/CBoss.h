#pragma once
#include "Player.h"
class CBullet;

class CBoss : public CPlayer
{
	friend class CSession;
private:
	CGameObject* m_RightHands = NULL;
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Rotate(float x, float y, float z);
	virtual void PrepareAnimate();
	virtual void Move(const int16& dwDirection, float fDistance);
	void SetAttackAnimOtherClient();
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	

	// ============= 애니메이션 트랙 셋팅 관련 ============
	void SetIdleAnimTrack();
	void SetRunAnimTrack();
	void SetAttackAnimTrack();
	void SetRunAttackAnimTrack();

	virtual void ProcessInput(const int16&); // 04-29 추가
};