#include "pch.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "Session.h"
#include "OBDC_MGR.h"


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
		case EventType::Connect:
		{
			ConnectEvent* connectEvent = static_cast<ConnectEvent*>(iocpEvent);
			DoRecv(); // Connect하고 Do recv 수행
		}
		break;
	case EventType::Recv:
		{
			RecvEvent* rev = static_cast<RecvEvent*>(iocpEvent);
			WRITE_LOCK;
			
			int remain_data = numOfBytes + _prev_remain;
			char* p = rev->_rbuf;
			while (remain_data > 0)
			{
				uint8 packet_size = p[0];
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
			
		}
		break;
	case EventType::Send:
		{
			SendEvent* sev = static_cast<SendEvent*>(iocpEvent);
			if(iocpEvent == nullptr) ASSERT_CRASH("double Del");
			delete iocpEvent;
		}
		break;
	}
}

void GameSession::DoSend(void* packet)
{
	
	DWORD sendLen(0);
	DWORD flag(0);
	SendEvent* sev = new SendEvent(reinterpret_cast<char*>(packet));
	sev->_sid = _sid;
	if (WSASend(_sock, &sev->_sWsaBuf, 1, &sendLen, flag, static_cast<LPWSAOVERLAPPED>(sev), NULL) == SOCKET_ERROR)
	{
		int32 errcode = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << errcode << endl;
		}
	}
}

void GameSession::DoRecv()
{
	_rev.Init();
	_rev._sid = _sid;
	_rev._cid = _cid;
	DWORD recvBytes(0);
	DWORD flag(0);
	if (WSARecv(_sock, &_rev._rWsaBuf, 1, &recvBytes, &flag, static_cast<LPWSAOVERLAPPED>(&_rev), NULL) == SOCKET_ERROR)
	{
		int32 errcode = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << errcode << endl;
		}
	}
}

void GameSession::DoSendLoginPacket(bool isSuccess)
{

	if (isSuccess)
	{
		S2C_LOGIN_OK loginOkPacket;
		loginOkPacket.size = sizeof(S2C_LOGIN_OK);
		loginOkPacket.type = S_PACKET_TYPE::LOGIN_OK;
		loginOkPacket.cid = _cid;
		DoSend(&loginOkPacket);
	}
	else
	{
		S2C_LOGIN_OK loginFailPacket;
		loginFailPacket.size = sizeof(S2C_LOGIN_OK);
		loginFailPacket.type = (uint8)S_PACKET_TYPE::LOGIN_FAIL;
		DoSend(&loginFailPacket);
	}
}

void GameSession::ProcessPacket(char* packet)
{
	switch ((uint8)packet[1])
	{
		// === CLIENT PACKET ===
	    //
	    // ===               ===
		case C_PACKET_TYPE::CCHAT:
		{

			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
			_CHAT np;
			memcpy(&np, cp, sizeof(_CHAT));
			np.type = SCHAT;
			for (auto i : GIocpCore._clients)
			{
				if (i.second->_cid == np.sid) continue;
				i.second->DoSend(&np);
			}
		}
		break;
		// === SERVER PACKET ===
		//
		// ===               ===
		case S_PACKET_TYPE::SCHAT:
		{
			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
			std::cout << "client[" << cp->sid << "] 's msg : " << cp->buf << std::endl;
		}
		break;
		case S_PACKET_TYPE::LOGIN_OK:
		{
			S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
			_cid = lo->cid;
		
			std::cout << "client[" << _cid << "] " << "Login Success" << std::endl;
			_status = STATUS::LOGIN;
		}
		break;
		case S_PACKET_TYPE::LOGIN_FAIL:
		{
			S2C_LOGIN_FAIL* lo = (S2C_LOGIN_FAIL*)packet;
			std::cout << "Login Fail" << std::endl;
			SocketUtil::Close(_sock);
		}
		break;
	}
	DoRecv();
}







