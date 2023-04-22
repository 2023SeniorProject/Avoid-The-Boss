#include "pch.h"
#include "CBullet.h"

CBullet::CBullet(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfo* pBulletModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Sphere.bin", NULL, Layout::Bullet);
	SetChild(pBulletModel->m_pModelRootObject, true);
}

CBullet::~CBullet()
{
}

void CBullet::Update(float fTimeElapsed)
{
	if (m_OnShoot)
		CGameObject::MoveForward(1.0f);
}
