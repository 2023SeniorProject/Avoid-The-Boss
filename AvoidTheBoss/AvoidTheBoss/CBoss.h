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
	virtual void Move(DWORD dwDirection, float fDistance);
	void AttackAnimationOn();
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);
	virtual void OnInteractionAnimation();

	virtual void ProcessInput(const int16&); // 04-29 Ãß°¡
};