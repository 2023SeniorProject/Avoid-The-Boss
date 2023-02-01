#pragma once
#include "IocpCore.h"
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�
class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
public:
	char _recvBuf[BUFSIZE];
	int32 _cid;
private:
	SOCKET _sock = INVALID_SOCKET;
};

