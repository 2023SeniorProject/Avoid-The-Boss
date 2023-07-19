#pragma once

// 오직 위치 정보만 가지고 있는 플레이어 클래스
// 렌더링과 관련된 정보 X

class SPlayer
{
public:
	//플레이어의 위치 벡터, x-축(Right), y-축(Up), z-축(Look) 벡터이다.
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look; // direction과 동일

	std::mutex _lock;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Velocity;

	// 05-24 추가 변수
	bool m_bIsRescue = false;
	
public:
	BoundingSphere m_playerBV;
	int32 m_sid = -1;
	int32 m_idx = -1;
	bool m_hide = false;
	int32 m_hp = 3; // 05-06추가 플레이어 HP
public:
	int32 m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;
	int32 m_attackedAnimationCount = 0;
	int32 m_downAnimationCount = 0;
private:
	bool m_isEscaped = false;
public:
	SPlayer();
	
	virtual ~SPlayer();

	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	XMFLOAT3 GetLook() const { return(m_xmf3Look); }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	
	void SetPosition(const XMFLOAT3& xmf3Position)
	{
		m_xmf3Position = xmf3Position;
	}
	void SetDirection(const XMFLOAT3& lookVec);

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }

	// 출력 함수
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

	// 행동 트리 함수
	void SetBehavior(PLAYER_BEHAVIOR pb) { m_behavior = (int32)pb; }
	int32 GetBehavior()					 { return m_behavior;	   }
	bool  GetEscaped()					 { return m_isEscaped;	   }

	void ProcessAttack() 
	{ 
		if(m_hp > 0) m_hp -= 1;
		if (m_hp <= 0)
		{
			SetBehavior(PLAYER_BEHAVIOR::CRAWL);
		}
	}
	void ProcessAlive()
	{
		SetBehavior(PLAYER_BEHAVIOR::IDLE);
		m_hp = 3;
	}


	void ProcessInput(const int16& input, const XMFLOAT3& lookVec);
	void Move(const int16& dwDirection, float fDistance);
	void Update(float fTimeElapsed);
	void LateUpdate(float fTimeElapsed);
	
	void ResetState();
};