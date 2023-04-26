#pragma once
#include "Player.h"


class CBullet;

/*class CBoss : public CPlayer
{
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback();
	virtual void OnCameraUpdateCallback();
	virtual void Move(DWORD dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);
	virtual void OnInteractive();
};*/


class CBoss : public CPlayer
{
public:
	int nBullet = 50;
	CBullet* m_pBullet = NULL;

	CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBoss();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback();
	virtual void OnCameraUpdateCallback();

	virtual void Move(DWORD dwDirection, float fDistance);

	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);
	virtual void OnInteractive();
};