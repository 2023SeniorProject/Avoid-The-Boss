#pragma once

// ���� ��ġ ������ ������ �ִ� �÷��̾� Ŭ����
// �������� ���õ� ���� X

class PlayerInfo
{
public:
	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�.
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look; // direction�� ����

	std::mutex _lock;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	XMFLOAT3 m_xmf3Position; 
	XMFLOAT3 m_xmf3Velocity;
public:
	PlayerInfo();
	virtual ~PlayerInfo();

	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position)
	{
		UpdateMove(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
	}
	void SetDirection(const XMFLOAT3& look) 
	{ 
		m_xmf3Look = look; 
		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	}
	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	

	void Rotate(float x, float y, float z);
	void Move(uint8 dwDirection, float fDistance);
	void UpdateMove(const XMFLOAT3& velocity);
	
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	void Update(float fTimeElapsed);
	void OnPlayerUpdateCallback(float fTimeElapsed) {};
};