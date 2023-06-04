#include "pch.h"
#include "SocketUtil.h"
#include "Session.h"
#include "CSIocpCore.h"
#include "JobQueue.h"
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
			DoRecv();
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
			delete sev;
			cout << "Send Error " << errcode << "\n";
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
	_rev._rWsaBuf.buf = _rev._rbuf + _prev_remain;
	_rev._rWsaBuf.len = BUFSIZE - _prev_remain;
	DWORD recvBytes(0);
	DWORD flag(0);
	
	if (WSARecv(_sock, &_rev._rWsaBuf, 1, &recvBytes, &flag, static_cast<LPWSAOVERLAPPED>(&_rev), NULL) == SOCKET_ERROR)
	{
		int32 errcode = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Recv Error " << errcode << "\n";
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
		loginOkPacket.type = (uint8)S_PACKET_TYPE::LOGIN_OK;
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
		case (uint8)C_PACKET_TYPE::CKEY:
		{
			// 키 패킷 처리
			C2S_KEY* movePacket = reinterpret_cast<C2S_KEY*>(packet);
			moveEvent* mv = new moveEvent(_sid, movePacket->key, XMFLOAT3{ movePacket->x,0,movePacket->z });
			queueEvent* me = static_cast<queueEvent*>(mv);
			
			// 서버키 패킷 전송
			S2C_KEY packet;
			packet.size = sizeof(S2C_KEY);
			packet.type = (uint8)S_PACKET_TYPE::SKEY;
			packet.sid = _sid;
			packet.key = movePacket->key;
			packet.x = movePacket->x;
			packet.z = movePacket->z;
			
			ServerIocpCore._rmgr->GetRoom(_myRm).AddEvent(me , 0.f);
			ServerIocpCore._rmgr->GetRoom(_myRm).BroadCastingExcept(&packet, _sid);
		}
		break;
		case (uint8)C_PACKET_TYPE::CROT:
		{
			C2S_ROTATE* rotatePacket = reinterpret_cast<C2S_ROTATE*>(packet);
			S2C_ROTATE packet;
			packet.size = sizeof(S2C_ROTATE);
			packet.type = (uint8)S_PACKET_TYPE::SROT;
			packet.sid = _sid;
			packet.angle = rotatePacket->angle;
			ServerIocpCore._rmgr->GetRoom(_myRm).BroadCastingExcept(&packet,_sid);

		}
		break;
		case (uint8)C_PACKET_TYPE::CCHAT:
		{

			_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
			_CHAT  np;
			memcpy(&np, cp, sizeof(_CHAT));
			np.type = (uint8)S_PACKET_TYPE::SCHAT;
	
			ServerIocpCore._rmgr->_rooms[_myRm].BroadCasting(&np);
		}
		break;
		// ======== 방 시스템 패킷
		case (uint8)C_ROOM_PACKET_TYPE::ACQ_ENTER_RM:
		{
			C2S_ROOM_ENTER* rep = reinterpret_cast<C2S_ROOM_ENTER*>(packet);
			ServerIocpCore._rmgr->EnterRoom(_sid,rep->rmNum);
		}
		break;
		case (uint8)C_ROOM_PACKET_TYPE::ACQ_MK_RM:
		{
			ServerIocpCore._rmgr->CreateRoom(_sid);
		}
		break;
		case (uint8)SC_PACKET_TYPE::GAMEEVENT:
		{
			SC_EVENTPACKET* ep = reinterpret_cast<SC_EVENTPACKET*>(packet);
			InteractionEvent* swev = new InteractionEvent();
			swev->eventId = ep->eventId;
			std::cout << "EVENT ID : " << (int32)swev->eventId << "\n";
 			swev->_sid = _sid;
			ServerIocpCore._rmgr->GetRoom(_myRm).AddEvent(swev, 0);
		}
		break;
	}
}

