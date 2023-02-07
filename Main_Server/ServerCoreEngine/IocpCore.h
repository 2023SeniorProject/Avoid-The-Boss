#pragma once
// Worker Thread ����� Ŭ������ ����
#include <array>

// IOCP�� ����� �� �ִ� ��� ������Ʈ�� ���ؼ� ����
class IocpObject 
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) abstract; // � �ϰ�����  Iocp�� ����ߴ�?
	// iocpEvent�� ���� ������ ���� ó��
};

class IocpCore
{

public:
	IocpCore();
	~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(class IocpObject* iocpObj); // socket�� session�� ����ϴ� �Լ�
	bool Processing(uint32_t limit_time = INFINITE); // ���������� ���ϴ� worker_thread���� iocp���� �Ϸ�� ������ Ž���ϴ� ��
	void Disconnect(int32 sid);
private:
	HANDLE _hIocp = INVALID_HANDLE_VALUE;
public:
	USE_LOCK;
	std::array<GameSession*, 1000> _clients;
};

extern IocpCore GIocpCore;

extern IocpCore ClientIocpCore;
