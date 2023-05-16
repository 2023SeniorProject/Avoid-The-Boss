#include "pch.h"
#include "GameObject.h"
#include "CBullet.h"

//CBullet::CBullet(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
//{
//	CLoadedModelInfo* pBulletModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Bullet.bin", NULL, Layout::BULLET);
//	SetChild(pBulletModel->m_pModelRootObject, true);
//}

CBullet::~CBullet()
{
}

void CBullet::Update(float fTimeElapsed)
{
	if (m_OnShoot)
	{
		if (m_fDistance > BUIIET_DISTANCE)
		{
			m_OnShoot = false;
			m_fDistance = 0.0f;
			m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		// 5.6 총알 발사 코드
		//XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		//xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
		//	BUIIET_DISTANCE);
		//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
		//
		//XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
		//SetPosition(Vector3::Add(GetPosition(), xmf3Velocity));
		
		m_fDistance += BULLET_SPEED;
	}
}

void CBullet::SetBulletPosition(XMFLOAT3 playerPos)
{
	SetPosition(playerPos);
}
