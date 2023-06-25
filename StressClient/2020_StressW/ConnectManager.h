#pragma once
#include "IocpObject.h"
#include "DummySession.h"
class DummyConnectEvent : public IocpEvent
{	
	
public:
	DummyConnectEvent() : IocpEvent(EventType::Connect)
	{
		Init();
		_client = new DummySession();
	}
public:
	char _buf[BUFSIZE / 2] = {};
	DummySession* _client;
};

class ConnectManager : public IocpObject
{
	using Clock = std::chrono::high_resolution_clock;
public:
	ConnectManager() = default;
	~ConnectManager();
public:
	bool BindAddress(char* address);
	bool InitConnect();

private:
	void RegisterConnect(ConnectEvent* acceptEvent);
	void ProcessConnect(ConnectEvent* acceptEvent);
	int32 GetNewSessionIdx();
public: 
	virtual HANDLE GetHandle() override { return HANDLE(); };
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	SOCKADDR_IN _serveraddr;
	std::vector<DummyConnectEvent*> _connectEvents;
	Atomic<int32> curConnectedCnt = 0;
	Clock::time_point _lastConnectedTime;
};

