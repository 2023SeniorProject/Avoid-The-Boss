#include "pch.h"
#include "GameObject.h"
#include "CBullet.h"

//CBullet::CBullet(ID3D12Device5* pd3dDevice, 
// 
// 
// * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
//{
//	CLoadedModelInfo* pBulletModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Bullet.bin", NULL, Layout::BULLET);
//	SetChild(pBulletModel->m_pModelRootObject, true);
//}

CBullet::CBullet()
{
}

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
	if (m_OnHit)
	{
		m_pHitEffect->SetPosition(GetPosition()); // 충돌 지점 위치 넘겨주기
		//+여기에 회전 위치 갱신 코드 추가하기
		XMFLOAT3 shootLook = XMFLOAT3(-GetLook().x, 0.0f, -GetLook().z);
		XMFLOAT3 effectLook = XMFLOAT3(m_pHitEffect->GetLook());
		float angle = Vector3::Angle(shootLook, effectLook);

		m_pHitEffect->Rotate(0.0f,-angle, 0.0f);
		m_pHitEffect->SetOnHit(true);
	}
}

void CBullet::SetBulletPosition(XMFLOAT3 playerPos)
{
	SetPosition(playerPos);
}
