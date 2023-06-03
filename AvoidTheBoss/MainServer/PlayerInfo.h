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
	int32 rescueTime = 5.0f;
public:
	BoundingSphere m_playerBV;
	int32 m_sid = -1;
	int32 m_idx = -1;
	bool m_hide = false;
	int32 m_hp = 5; // 05-06�߰� �÷��̾� HP
public:
	int32 m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;
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
		m_xmf3Position = xmf3Position;
	}
	void SetDirection(const XMFLOAT3& lookVec);

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

	void ProcessInput(const int16& input, const XMFLOAT3& lookVec);
	void Move(const int16& dwDirection, float fDistance);
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	void Update(float fTimeElapsed);
	void LateUpdate(float fTimeElapsed);
};