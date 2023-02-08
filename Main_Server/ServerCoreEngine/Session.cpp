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
			WLock;
			RecvEvent* rev = static_cast<RecvEvent*>(iocpEvent);
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
#pragma region CLIENT to SERVER PACKET
		case C_PACKET_TYPE::CCHAT:
		{

			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
		
			cp->type = SCHAT;
			READ_IOCP_LOCK;
			/*for(auto& i : GIocpCore._clients)
			{
				if (i.second->_cid == cp->cid) continue;
				i.second->DoSend(cp);
			}*/
			GIocpCore.rMgr->_rooms[_myRm].BroadCasting(cp);
		}
		break;
		// ======== 방 시스템 패킷
		case C_ROOM_PACKET_TYPE::ACQ_ENTER_RM:
		{
			C2S_ROOM_ENTER* rep = reinterpret_cast<C2S_ROOM_ENTER*>(packet);
			GIocpCore.rMgr->EnterRoom(_sid,rep->rmNum);
		}
		break;
		case C_ROOM_PACKET_TYPE::ACQ_MK_RM:
		{
			GIocpCore.rMgr->CreateRoom(_sid);
		}
		break;
#pragma endregion
#pragma region SERVER to CLIENT PACKET 
		case S_PACKET_TYPE::SCHAT:
		{
			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
			std::cout << "client[" << cp->cid << "] 's msg : " << cp->buf << std::endl;
		}
		break;
		case S_PACKET_TYPE::LOGIN_OK:
		{
			S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
			_cid = lo->cid;
		
			std::cout << "client[" << _cid << "] " << "Login Success" << std::endl;
			_status = USER_STATUS::LOBBY;
		}
		break;
		case S_PACKET_TYPE::LOGIN_FAIL:
		{
			S2C_LOGIN_FAIL* lo = (S2C_LOGIN_FAIL*)packet;
			std::cout << "Login Fail" << std::endl;
			SocketUtil::Close(_sock);
		}
		break;
        // ===== 방 관련 패킷 ============
		case S_ROOM_PACKET_TYPE::REP_ENTER_RM:
		{
			S2C_ROOM_ENTER* re = (S2C_ROOM_ENTER*)packet;
			if (re->success)
			{
				_curScene = 1;
				_status = USER_STATUS::ROOM;
				::system("cls");
			}
			else std::cout << "FAIL TO ENTER ROOM" << std::endl;
 		}
		break;
		case S_ROOM_PACKET_TYPE::MK_RM_FAIL:
		{
			std::cout << "Fail to Create Room!!(MAX_CAPACITY)" << std::endl;
		}
		break;

#pragma endregion
	}
	DoRecv();
}







