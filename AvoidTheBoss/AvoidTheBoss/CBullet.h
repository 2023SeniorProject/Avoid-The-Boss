#pragma once


#define BUIIET_DISTANCE 2.5f
#define BULLET_NUMBER 1

class GameObject;

class CBullet : public CGameObject
{
private:
	float		m_fSpeed = 0.5f;
	float		m_fDistance = 0.0f;
	XMFLOAT3	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	bool		m_OnShoot = false;
public:
	CBullet() {};
	//CBullet(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CBullet();

	virtual void Update(float fTimeElapsed);
	void SetOnShoot(bool value) { m_OnShoot = value; }
	bool GetOnShoot() { return m_OnShoot; }

	void SetBulletPosition(XMFLOAT3 Pos);
};


