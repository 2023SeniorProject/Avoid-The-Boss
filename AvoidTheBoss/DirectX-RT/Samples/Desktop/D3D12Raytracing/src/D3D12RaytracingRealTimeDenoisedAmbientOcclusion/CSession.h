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
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
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