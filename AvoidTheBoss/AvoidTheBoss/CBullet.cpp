#include "pch.h"
#include "GameObject.h"
#include "CBullet.h"

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
		//5.6 총알 발사 코드
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			BUIIET_DISTANCE);
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);

		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);

		SetPosition(Vector3::Add(GetPosition(), xmf3Velocity));

		m_fDistance += BULLET_SPEED;
		if (m_fDistance > BUIIET_DISTANCE) 
		{
			m_OnShoot = false;
			m_OnHit = true; //이 값을 총알 충돌 판정 처리로 옮긴다.

			m_fDistance = 0.0f;
			m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}
	if (m_OnHit)
	{
		m_pHitEffect->SetPosition(GetPosition().x, 1.0f, GetPosition().z); // 충돌 지점 위치 넘겨주기
		m_pHitEffect->SetOnHit(true);
		m_OnHit = false;
	}
}

void CBullet::SetBulletPosition(XMFLOAT3 playerPos)
{
	SetPosition(playerPos);
}
