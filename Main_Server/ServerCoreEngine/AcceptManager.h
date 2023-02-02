#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
// Accept를 진행할 싱글톤 객체 (Listener)

class AcceptManager : public IocpObject
{
public:
	AcceptManager() = default;
	~AcceptManager();
public:
	// Accept를 받을 준비를 진행해라
	bool InitAccept();
	void CloseSocket();
private:
	// 수신관련 진행
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
public: // 인터페이스 구현할 예정
	// 상속하고 있는 iocObject의 추상 함수들을 오버라이딩
	virtual HANDLE GetHandle() override;
	virtual void Processing(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	SOCKET _listenSock = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};

