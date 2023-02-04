#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�

void ProcessPacket(int32 key, char* packet);

enum class STATUS: int8 { EMPTY, LOGIN, INGAME};


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
	void DoSendLoginPacket(bool isSuccess);
	void ProcessPacket(char* packet);
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int32 _prev_remain = 0;
	STATUS _status = STATUS::EMPTY;
public:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;
	
	USE_LOCK;
};

