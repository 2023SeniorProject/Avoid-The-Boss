#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType et) : _comp(et) 
{
	Init();
}

void IocpEvent::Init()
{
	// 오버랩드 구조체를 초기화 시킨다.
	WSAOVERLAPPED::hEvent = 0;
	WSAOVERLAPPED::Offset = 0;
	WSAOVERLAPPED::OffsetHigh = 0;
	WSAOVERLAPPED::Internal = 0;
	WSAOVERLAPPED::InternalHigh = 0;
}

