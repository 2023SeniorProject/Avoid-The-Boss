#include "pch.h"
#include "CorePlayer.h"

CorePlayer::CorePlayer() 
{
	

	_playerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	_pRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	_pUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	_pLook = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	
}

CorePlayer::~CorePlayer()
{
}

/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�. �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带
���� �� ����ȴ�. �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/
void CorePlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		////ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�. 
		//if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
		//	fDistance);
		//if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
		//	-fDistance);

		////ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�. 
		//if (dwDirection & DIR_RIGHT)
		//	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
		//		fDistance);
		//if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
		//	-fDistance);

		////��Page Up���� ������ ���� y-�� �������� �̵��Ѵ�. ��Page Down���� ������ �ݴ� �������� �̵��Ѵ�. 
		//if (dwDirection & DIR_UP)
		//	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		//if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up,
		//	-fDistance);

		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
		SetSpeed(xmf3Shift);
	}
}

void CorePlayer::SetSpeed(const XMFLOAT3& velocity)
{
	
	//�÷��̾��� �ӵ� ���͸� xmf3Shift ���͸�ŭ �����Ѵ�. 
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, velocity);
}

void CorePlayer::UpdateMove(const XMFLOAT3& xmf3Shift)
{
	//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
	_playerPos = Vector3::Add(_playerPos, xmf3Shift);
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
void CorePlayer::Rotate(float x, float y, float z)
{
}



//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�.
void CorePlayer::Update(float fTimeElapsed)
{
	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);

	UpdateMove(xmf3Velocity);

	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ����
	�ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ�
	������ �� �ִ�.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}









