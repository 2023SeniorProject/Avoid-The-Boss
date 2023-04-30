#pragma once
#include "IocpEvent.h"

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) abstract; // 어떤 일감으로  Iocp에 등록했니?
	// iocpEvent에 따라 적절한 일을 처리
};
