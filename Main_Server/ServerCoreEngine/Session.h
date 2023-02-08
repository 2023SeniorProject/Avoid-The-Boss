#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�


enum class USER_STATUS: int8 { EMPTY,LOBBY,ROOM,INGAME};



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
	void ProcessPacket(char*);
public:
	int16 _cid = -1;
	int16 _myRm = -1;
	int16 _sid = -1;
	int32 _prev_remain = 0;
	Atomic<USER_STATUS> _status = USER_STATUS::EMPTY;
	int8 _curScene = 0;
public:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;
	RWLOCK;
};

