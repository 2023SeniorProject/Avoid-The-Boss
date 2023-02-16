#include "pch.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "Session.h"
#include "packetEvent.h"
#include "OBDC_MGR.h"

using namespace std;
// =========== 서버 세션 ============

ServerSession::ServerSession() 
{
	_sock = SocketUtil::CreateSocket();
}                   

ServerSession::~ServerSession()
{
	SocketUtil::Close(_sock);
}

HANDLE ServerSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_sock);
}

void ServerSession::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->_comp)
	{
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
			if (iocpEvent == nullptr) ASSERT_CRASH("double Del");
			delete iocpEvent;
		}
		break;
	}
	
}

bool ServerSession::DoSend(void* packet)
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
			return false;
		}
	}
	return true;
}

bool ServerSession::DoRecv()
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
			return false;
		}
	}
	return true;
}

void ServerSession::DoSendLoginPacket(bool isSuccess)
{

	if (isSuccess)
	{
		S2C_LOGIN_OK loginOkPacket;
		loginOkPacket.size = sizeof(S2C_LOGIN_OK);
		loginOkPacket.type = S_PACKET_TYPE::LOGIN_OK;
		loginOkPacket.cid = _cid;
		loginOkPacket.sid = _sid;
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

void ServerSession::ProcessPacket(char* packet)
{
	switch ((uint8)packet[1])
	{
		case C_PACKET_TYPE::MOVE:
		{
			std::lock_guard<std::mutex> pl(_playerLock); // 플레이어 정보 갱신
			C2S_MOVE* movePacket = reinterpret_cast<C2S_MOVE*>(packet);
			
			XMFLOAT3 velocity;
			velocity.x = movePacket->x;
			velocity.y = movePacket->y;
			velocity.z = movePacket->z;

			moveEvent* mv = new moveEvent;
			mv->type = EVENT_TYPE::MOVE_EVENT;
			mv->velocity = velocity;
			mv->sid = _sid;
			queueEvent* me = static_cast<queueEvent*>(mv);

			// move 패킷 브로드 캐스팅
			S2C_MOVE packet;
			packet.size = sizeof(S2C_MOVE);
			packet.type = S_PACKET_TYPE::SMOVE;
			packet.sid = _sid;
			packet.x  = velocity.x;
			packet.y  = velocity.y;
			packet.z  = velocity.z;
			ServerIocpCore._rmgr->_rooms[_myRm].BroadCasting(&packet);
			ServerIocpCore._rmgr->_rooms[_myRm].AddEvent(me);
		}
		break;
		case C_PACKET_TYPE::CCHAT:
		{

			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
			_CHAT  np;
			memcpy(&np, cp, sizeof(_CHAT));
			np.type = S_PACKET_TYPE::SCHAT;
			READ_SERVER_LOCK;
			ServerIocpCore._rmgr->_rooms[_myRm].BroadCasting(&np);
		}
		break;
		// ======== 방 시스템 패킷
		case C_ROOM_PACKET_TYPE::ACQ_ENTER_RM:
		{
			C2S_ROOM_ENTER* rep = reinterpret_cast<C2S_ROOM_ENTER*>(packet);
			ServerIocpCore._rmgr->EnterRoom(_sid,rep->rmNum);
		}
		break;
		case C_ROOM_PACKET_TYPE::ACQ_MK_RM:
		{
			ServerIocpCore._rmgr->CreateRoom(_sid);
		}
		break;
	}
	DoRecv();
}


// ========== 클라이언트 세션 ============

// =======================================
ClientSession::ClientSession()
{
	_sock = SocketUtil::CreateSocket();
}

ClientSession::~ClientSession()
{
	SocketUtil::Close(_sock);
}

HANDLE ClientSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_sock);
}

void ClientSession::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->_comp)
	{
	case EventType::Connect:
	{
		ConnectEvent* connectEvent = static_cast<ConnectEvent*>(iocpEvent);
		delete connectEvent;
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
		if (iocpEvent == nullptr) ASSERT_CRASH("double Del");
		delete iocpEvent;
	}
	break;
	}
}

bool ClientSession::DoSend(void* packet)
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
			return false;
		}
	}
	return true;
}

bool ClientSession::DoRecv()
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
			return false;
		}
	}
	return true;
}

void ClientSession::ProcessPacket(char* packet)
{
	switch ((uint8)packet[1])
	{

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
			_sid = lo->sid;
			_status = USER_STATUS::LOBBY;
			_curScene = 0;
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
	}
	DoRecv();
}
