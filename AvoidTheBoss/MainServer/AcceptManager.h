#pragma once
#include "IocpObject.h"
#include "NetAddress.h"

class AcceptEvent;
// Accept�� ������ �̱��� ��ü (Listener)

class AcceptManager : public IocpObject
{
public:
	AcceptManager() = default;
	~AcceptManager();
public:
	// Accept�� ���� �غ� �����ض�
	bool InitAccept();
	void CloseSocket();
private:
	// ���Ű��� ����
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
	int32 GetNewSessionIdx();
public: // �������̽� ������ ����
	// ����ϰ� �ִ� iocObject�� �߻� �Լ����� �������̵�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	SOCKET _listenSock = INVALID_SOCKET;
	std::vector<AcceptEvent*> _acceptEvents;
	
	int32 maxAcceptCnt = 1000;
	Atomic<int32> curAcceptCnt = 0;
};
