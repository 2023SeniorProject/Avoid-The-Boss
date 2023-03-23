#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "PlayerInfo.h"
// �������� Ŭ���̾�Ʈ ������ ������ Ŭ����
// ���������� Iocp�� ����� ����̱� ������ IocpObject�� �ش�ȴ�


enum class USER_STATUS: int8 { EMPTY,LOBBY,ROOM,INGAME};

class ServerSession : public IocpObject
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
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	Atomic<USER_STATUS> _status = USER_STATUS::EMPTY;
	
public:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;

	std::mutex _playerLock; // �÷��̾� ������ Lock
	PlayerInfo _playerInfo;
};


