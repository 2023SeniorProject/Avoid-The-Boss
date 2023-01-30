#pragma once
#include "IocpCore.h"

class AcceptManager : public IocpObject
{
	AcceptManager() {}
	~AcceptManager() {}

	bool InitAccept();
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
public:
	virtual HANDLE GetHandle() override;
	virtual void CheckCompletionWork(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	SOCKET _listenSock = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};

