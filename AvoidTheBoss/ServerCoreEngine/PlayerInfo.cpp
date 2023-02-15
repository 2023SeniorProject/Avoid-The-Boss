#include "pch.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() 
{
	

	_playerPos = XMFLOAT3(0.0f, 75.0f, 0.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pPlayerUpdatedContext = NULL;
	
}

PlayerInfo::~PlayerInfo()
{
}

void PlayerInfo::UpdateMove(const XMFLOAT3& xmf3Shift)
{
	//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
	_playerPos = Vector3::Add(_playerPos, xmf3Shift);
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.



//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다.
void PlayerInfo::Update(float fTimeElapsed)
{
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	//std::cout << xmf3Velocity.x << " " << xmf3Velocity.z << " " << fTimeElapsed << std::endl;
	UpdateMove(xmf3Velocity);
	PrintPlayerInfo();
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

}









