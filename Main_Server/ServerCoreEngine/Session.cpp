#include "pch.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "Session.h"

GameSession::GameSession() 
{
	_sock = SocketUtil::CreateSocket();
}

GameSession::~GameSession()
{
	SocketUtil::Close(_sock);
}

HANDLE GameSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_sock);
}

void GameSession::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//TODO
}

void GameSession::DoSend(void* packet)
{
	SendEvent* sev;
	DWORD sendLen(0);
	DWORD flag(0);
	sev = new SendEvent(reinterpret_cast<char*>(packet));
	sev->_sid = _sid;
	WSASend(_sock, &sev->_sWsaBuf, 1, &sendLen, flag, static_cast<LPWSAOVERLAPPED>(sev), NULL);
}

void GameSession::DoRecv()
{
	ZeroMemory(&_rev, sizeof(WSAOVERLAPPED));
	_rev._sid = _sid;
	_rev._cid = _cid;
	DWORD recvBytes(0);
	DWORD flag(0);
	WSARecv(_sock, &_rev._rWsaBuf, 1, &recvBytes, &flag, static_cast<LPWSAOVERLAPPED>(&_rev), NULL);
}
