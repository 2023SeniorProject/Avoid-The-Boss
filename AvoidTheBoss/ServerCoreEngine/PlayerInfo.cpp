#include "pch.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() 
{
	

	_playerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);


	m_pPlayerUpdatedContext = NULL;
	
}

PlayerInfo::~PlayerInfo()
{
}

/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를
누를 때 변경된다. 플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/
void PlayerInfo::Move(float fDistance = UNIT * 1.2f)
{
	
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 

	
}

void PlayerInfo::SetSpeed(const XMFLOAT3& velocity)
{
	//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다. 
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, velocity);
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

	UpdateMove(xmf3Velocity);

	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 플레이어의 새로운 위치가 유효한 위치가 아닐 수도
	있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다. 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시
	변경할 수 있다.*/
	PrintPlayerInfo();
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}









