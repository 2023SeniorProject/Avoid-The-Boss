#pragma once
#include "IocpEvent.h"

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) abstract; // � �ϰ�����  Iocp�� ����ߴ�?
	// iocpEvent�� ���� ������ ���� ó��
};
