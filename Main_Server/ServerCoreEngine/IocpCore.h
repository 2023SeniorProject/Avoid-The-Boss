#pragma once
#pragma once

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void CheckCompletionWork(class IocpEvent* iev, int32 numbytes) abstract;
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(IocpObject* iocpObj);
	bool CheckCompletionWork(uint32_t limit_time = INFINITE);
private:
	HANDLE _hIocp = INVALID_HANDLE_VALUE;
};

extern IocpCore GIocpCore;

