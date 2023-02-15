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
	//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
	_playerPos = Vector3::Add(_playerPos, xmf3Shift);
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.



//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�.
void PlayerInfo::Update(float fTimeElapsed)
{
	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	//std::cout << xmf3Velocity.x << " " << xmf3Velocity.z << " " << fTimeElapsed << std::endl;
	UpdateMove(xmf3Velocity);
	PrintPlayerInfo();
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

}









