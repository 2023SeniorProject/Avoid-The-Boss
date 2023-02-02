#pragma once
#include "IocpCore.h"
class RecvEvent;
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�
class GameSession : public IocpObject
{
public:
	GameSession();
	virtual ~GameSession();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
	void DoSend(void* packet);
	void DoRecv();
	void ProcessPacket(char* packet);
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int32 _prev_remain = 0;
private:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;
	SendEvent* _sev;
	USE_LOCK;
};

