#pragma once
#include "Player.h"
class CBullet;

class CBoss : public CPlayer
{
private:
	CGameObject* m_RightHands = NULL;
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback();
	virtual void OnCameraUpdateCallback();
	
	virtual void Rotate(float x, float y, float z);
	virtual void PrepareAnimate();
	virtual void Move(DWORD dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);

	virtual void ProcessInput(DWORD&); // 04-29 Ãß°¡
};