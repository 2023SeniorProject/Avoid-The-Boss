#pragma once

// 오직 위치 정보만 가지고 있는 플레이어 클래스
// 렌더링과 관련된 정보 X

class PlayerInfo
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
	
	//플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수이다.
	void Update(float fTimeElapsed);
	void OnPlayerUpdateCallback(float fTimeElapsed) {};
};