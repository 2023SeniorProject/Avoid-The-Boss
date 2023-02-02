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
	switch (iocpEvent->_comp)
	{
	case EventType::Recv:
		{
			WRITE_LOCK;
			RecvEvent* rev = static_cast<RecvEvent*>(iocpEvent);
			int remain_data = numOfBytes + _prev_remain;
			char* p = rev->_rbuf;
			while (remain_data > 0)
			{
				int8 packet_size = p[0];
				if (packet_size <= remain_data)
				{
					ProcessPacket(p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			_prev_remain = remain_data;
			if (remain_data > 0) 
			{
				memcpy(rev->_rbuf, p, remain_data);
			}
			DoRecv();
		}
		break;
	case EventType::Send:
		{
			WRITE_LOCK;
			if(_sev == nullptr) ASSERT_CRASH("double Del");
			delete _sev;
		}
		break;
	}
}

void GameSession::DoSend(void* packet)
{
	
	DWORD sendLen(0);
	DWORD flag(0);
	_sev = new SendEvent(reinterpret_cast<char*>(packet));
	_sev->_sid = _sid;
	WSASend(_sock, &_sev->_sWsaBuf, 1, &sendLen, flag, static_cast<LPWSAOVERLAPPED>(_sev), NULL);
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
