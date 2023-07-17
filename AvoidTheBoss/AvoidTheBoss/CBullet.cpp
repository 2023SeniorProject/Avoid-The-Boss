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
		if (m_fDistance > BUIIET_DISTANCE)
		{
			m_pHitEffect->SetPosition(GetPosition().x, 1.0f,GetPosition().z); // �浹 ���� ��ġ �Ѱ��ֱ�
			//m_pHitEffect->SetDirection(GetLook());
			//std::cout << "�Ѿ�-����Ʈ ȸ�� ���� : " << GetLook() << std::endl;

			//m_pHitEffect->Rotate(0.0f, 90.0f, 0.0f);

			m_OnShoot = false;
			m_OnHit = true;

			m_fDistance = 0.0f;
			m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		 //5.6 �Ѿ� �߻� �ڵ�
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			BUIIET_DISTANCE);
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
		
		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
		SetPosition(Vector3::Add(GetPosition(), xmf3Velocity));

		m_fDistance += BULLET_SPEED;
	}
	if (m_OnHit)
	{
		//+���⿡ ȸ�� ��ġ ���� �ڵ� �߰��ϱ�
		//XMFLOAT3 shootLook = XMFLOAT3(GetLook());
		//XMFLOAT3 effectLook = XMFLOAT3(m_pHitEffect->GetLook());
		//float angle = Vector3::Angle(shootLook, effectLook);
		

		std::cout << "�Ѿ� : " << GetPosition().x<<"," << GetPosition().y << "," << GetPosition().z << std::endl;

		std::cout << "Hit : " << m_pHitEffect->GetPosition().x << "," << m_pHitEffect->GetPosition().y << "," << m_pHitEffect->GetPosition().z << std::endl;

		m_pHitEffect->SetOnHit(true);
		m_OnHit = false;
	}
	m_pHitEffect->Update(fTimeElapsed);
}

void CBullet::SetBulletPosition(XMFLOAT3 playerPos)
{
	SetPosition(playerPos);
}
