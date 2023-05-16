#pragma once
// Worker Thread ����� Ŭ������ ����
//#include "RoomManager.h"
// IOCP�� ����� �� �ִ� ��� ������Ʈ�� ���ؼ� ����


class IocpObject;
class IocpCore
{

public:
	IocpCore();
	virtual ~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(class IocpObject* iocpObj); // socket�� session�� ����ϴ� �Լ�
	bool Processing(uint32_t limit_time = INFINITE); // ���������� ���ϴ� worker_thread���� iocp���� �Ϸ�� ������ Ž���ϴ� ��
	virtual void Disconnect(int32 sid);
protected:
	HANDLE _hIocp = INVALID_HANDLE_VALUE;
};

