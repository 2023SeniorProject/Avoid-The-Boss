#pragma once
#include "Asession.h"

class IocpEvent;
class Scheduler;

class DXSample;

class CSession : public ASession
{

public:
	CSession();
	virtual ~CSession();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
	void SetGame(DXSample*);
public:
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int16  _loginOk = -3;
	DXSample* _sample;
private:
	Scheduler* _DelayjobQueue;
	std::shared_mutex _DelayQueueLock;

};