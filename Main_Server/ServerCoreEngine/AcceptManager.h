#pragma once
#include "IocpCore.h"
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
public: // �������̽� ������ ����
	// ����ϰ� �ִ� iocObject�� �߻� �Լ����� �������̵�
	virtual HANDLE GetHandle() override;
	virtual void Processing(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	SOCKET _listenSock = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};

