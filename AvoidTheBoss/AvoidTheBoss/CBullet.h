#pragma once
#include "GameObject.h"

class CBullet : public CGameObject
{
private:
	bool m_OnShoot = false;
public:
	CBullet(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBullet();

	virtual void Update(float fTimeElapsed);
	void SetOnShoot(bool value) { m_OnShoot = value; }
	bool GetOnShoot() { return m_OnShoot; }
};


