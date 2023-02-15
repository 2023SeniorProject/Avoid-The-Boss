#pragma once

// ���� ��ġ ������ ������ �ִ� �÷��̾� Ŭ����
// �������� ���õ� ���� X

class PlayerInfo
{
protected:
	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�. 
	XMFLOAT3 _playerPos;
	//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�. 
	XMFLOAT3 m_xmf3Velocity;
	//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	XMFLOAT3 m_xmf3Gravity;

	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� �������̴�.
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
	
	//�÷��̾ �̵��ϴ� �Լ��̴�. 
	void UpdateMove(const XMFLOAT3& velocity);
	
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	void Update(float fTimeElapsed);
	void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void PrintPlayerInfo() { std::cout << "(" << _playerPos.x <<", " << _playerPos.z <<" )" << std::endl; }
};