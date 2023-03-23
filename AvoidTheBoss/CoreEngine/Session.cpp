#include "pch.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "Session.h"
#include "packetEvent.h"
#include "OBDC_MGR.h"

using namespace std;
// =========== ���� ���� ============

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

			C2S_MOVE* movePacket = reinterpret_cast<C2S_MOVE*>(packet);
			
			uint8 key;
			key = movePacket->key;

			moveEvent* mv = new moveEvent;
			mv->key = key;
			mv->sid = _sid;
			queueEvent* me = static_cast<queueEvent*>(mv);

			// move ��Ŷ ��ε� ĳ����
			S2C_MOVE packet;
			packet.size = sizeof(S2C_MOVE);
			packet.type = S_PACKET_TYPE::SMOVE;
			packet.sid = _sid;
			packet.key = key;
			ServerIocpCore._rmgr->GetRoom(_myRm).BroadCasting(&packet);
			ServerIocpCore._rmgr->GetRoom(_myRm).AddEvent(me);
			//ServerIocpCore._rmgr->_rooms[_myRm].BroadCasting(&packet);
			//ServerIocpCore._rmgr->_rooms[_myRm].AddEvent(me);
		}
		break;
		case C_PACKET_TYPE::ROTATE:
		{
	
			C2S_ROTATE* rotatePacket = reinterpret_cast<C2S_ROTATE*>(packet);

			rotateEvent* mv = new rotateEvent;
			mv->angleY = rotatePacket->angle;
			mv->sid = _sid;
			queueEvent* me = static_cast<queueEvent*>(mv);

	
			S2C_ROTATE packet;
			packet.size = sizeof(S2C_ROTATE);
			packet.type = S_PACKET_TYPE::SROTATE;
			packet.sid = _sid;
			packet.angle = rotatePacket->angle;

			ServerIocpCore._rmgr->GetRoom(_myRm).BroadCasting(&packet);
			ServerIocpCore._rmgr->GetRoom(_myRm).AddEvent(me);
			//ServerIocpCore._rmgr->_rooms[_myRm].BroadCasting(&packet);
			//ServerIocpCore._rmgr->_rooms[_myRm].AddEvent(me);

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
		// ======== �� �ý��� ��Ŷ
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
