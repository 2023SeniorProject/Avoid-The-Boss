#pragma once

// 오직 위치 정보만 가지고 있는 플레이어 클래스
// 렌더링과 관련된 정보 X

class PlayerInfo
{
protected:
	//플레이어의 위치 벡터, x-축(Right), y-축(Up), z-축(Look) 벡터이다. 
	XMFLOAT3 _playerPos;
	//플레이어의 이동 속도를 나타내는 벡터이다. 
	XMFLOAT3 m_xmf3Velocity;
	//플레이어에 작용하는 중력을 나타내는 벡터이다.
	XMFLOAT3 m_xmf3Gravity;

	//플레이어의 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pPlayerUpdatedContext;
	
public:
	PlayerInfo();
	virtual ~PlayerInfo();

	XMFLOAT3 GetPosition() const { return(_playerPos); }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position)
	{
		UpdateMove(XMFLOAT3(xmf3Position.x - _playerPos.x, xmf3Position.y - _playerPos.y, xmf3Position.z - _playerPos.z));
	}

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	
	//플레이어를 이동하는 함수이다. 
	void UpdateMove(const XMFLOAT3& velocity);
	
	//플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수이다.
	void Update(float fTimeElapsed);
	void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void PrintPlayerInfo() { std::cout << "(" << _playerPos.x <<", " << _playerPos.z <<" )" << std::endl; }
};