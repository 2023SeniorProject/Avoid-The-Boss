#include "pch.h"
#include "SocketUtil.h"
#include "Session.h"

Session::Session()
{
	_sock = SocketUtil::CreateSocket();
}

Session::~Session()
{
	SocketUtil::Close(_sock);
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_sock);
}

void Session::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//TODO
}
