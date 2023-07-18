#pragma once
#include "GameObject.h"
#define BUIIET_DISTANCE 2.5f
#define BULLET_NUMBER 1
#define BULLET_SPEED 0.1f


class CBullet : public CGameObject
{
private:
	float		m_fDistance = 0.0f;
	XMFLOAT3	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	bool		m_bStartShoot = false;
	bool		m_OnShoot = false;
	bool		m_OnHit = false;
public:
	CHitEffect* m_pHitEffect = NULL;

	CBullet();
	virtual ~CBullet();

	virtual void Update(float fTimeElapsed);
	void SetStartShoot(bool value) { m_bStartShoot = value; }
	bool GetStartShoot() { return m_bStartShoot; }

	void SetOnShoot(bool value) { m_OnShoot = value; }
	bool GetOnShoot() { return m_OnShoot; }

	void SetOnHit(bool value) { m_OnHit = value; }
	bool GetOnHit() { return m_OnHit; }

	CHitEffect* GetHitEffect() { return m_pHitEffect; }
	void SetHitEffect(CHitEffect* pHit) { m_pHitEffect = pHit; }

	void SetBulletPosition(XMFLOAT3 Pos);
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	void SetDirection(const XMFLOAT3& look)
	{
		m_xmf3Look = look;
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	}
};


