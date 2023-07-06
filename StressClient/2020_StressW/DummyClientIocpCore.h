#pragma once
#include "IocpCore.h"

class DummyClientIocpCore : public IocpCore
{
private:
	SOCKADDR_IN _serveraddr;
	static DummyClientIocpCore* _instance;
public:
	static DummyClientIocpCore& GetInstance()
	{
		if (_instance == nullptr) _instance = new DummyClientIocpCore();
		return *_instance;
	}
private:
	DummyClientIocpCore(const DummyClientIocpCore& ref) {}
	DummyClientIocpCore& operator=(const DummyClientIocpCore& ref) {}
	DummyClientIocpCore() {}
	~DummyClientIocpCore() {}

};