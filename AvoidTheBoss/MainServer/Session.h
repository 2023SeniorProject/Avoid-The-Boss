#pragma once

#include "IocpEvent.h"
#include "ASession.h"
#include "PlayerInfo.h"
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�


enum class USER_STATUS: int8 { EMPTY,LOBBY,ROOM,INGAME};

class ServerSession : public ASession
{
public:
	ServerSession();
	virtual ~ServerSession();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void DoSendLoginPacket(bool isSuccess);
	void ProcessPacket(char*);
public:
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	Atomic<USER_STATUS> _status = USER_STATUS::EMPTY;
	
public:
	//SOCKET _sock = INVALID_SOCKET;
	//RecvEvent _rev;
	//std::mutex _playerLock; // �÷��̾� ������ Lock
};


