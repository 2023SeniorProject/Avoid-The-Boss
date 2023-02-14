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

/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�. �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带
���� �� ����ȴ�. �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/
void PlayerInfo::Move(float fDistance = UNIT * 1.2f)
{
	
		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 

	
}

void PlayerInfo::SetSpeed(const XMFLOAT3& velocity)
{
	//�÷��̾��� �ӵ� ���͸� xmf3Shift ���͸�ŭ �����Ѵ�. 
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, velocity);
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

	UpdateMove(xmf3Velocity);

	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ����
	�ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ�
	������ �� �ִ�.*/
	PrintPlayerInfo();
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}









