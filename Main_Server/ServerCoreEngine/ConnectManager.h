#pragma once
#include "IocpCore.h"

class GameSession;

class ConnectManager : public IocpObject
{
public: // �������̽� ������ ����
	// ����ϰ� �ִ� iocObject�� �߻� �Լ����� �������̵�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	void InitConnect(GameSession& csession, const char* address);
	void DoConnect(const char* idPw);
private:
	SOCKADDR_IN _serveraddr;
	SOCKET _sock;
};

