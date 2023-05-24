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

	// 05-24 �߰� ����
	bool m_bIsAwaking = false;
public:
	BoundingSphere m_playerBV;
	int32 m_sid = -1;
	int32 m_idx = -1;
	bool m_hide = false;
	int32 m_hp = 5; // 05-06�߰� �÷��̾� HP
public:
	int32 m_behavior = IDLE;
	int32 m_attackedAnimationCount = 0;
	int32 m_downAnimationCount = 0;
public:
	PlayerInfo();
	virtual ~PlayerInfo();

	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	
	void SetPosition(const XMFLOAT3& xmf3Position)
	{
		m_xmf3Look = xmf3Position;
	}
	void SetDirection(const XMFLOAT3 look);

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }

	void PrintRightVec()
	{
		std::cout << "(" << m_xmf3Right.x << " " << m_xmf3Right.z << ")\n";
	}
	void PrintVel()
	{
		std::cout << m_xmf3Velocity.x << " " << m_xmf3Velocity.z << "\n";
	}

	void PrintPos()
	{
		std::cout << m_xmf3Position.x << " " << m_xmf3Position.z << "\n";
	}

	void Rotate(float x, float y, float z);
	void Move(uint8 dwDirection, float fDistance);
	void UpdateMove(const XMFLOAT3& velocity);
	
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	void Update(float fTimeElapsed);
	void LateUpdate();
};