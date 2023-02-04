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
					//ProcessPacket(_cid , p);
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


//void DB_Worker(int32 key, wstring sqlexec)
//{
//	USER_DB_MANAGER udb;
//	udb.AllocateHandles();
//	udb.ConnectDataSource(L"2023SENIORPROJECT");
//	const WCHAR* a = sqlexec.c_str();
//	udb.ExecuteStatementDirect(a);
//	udb.RetrieveResult();
//
//	lockG lg(m);
//	{
//		clients[key]->_cid = udb.user_cid;
//		if (clients[key]->_cid == -1)
//		{
//			cout << "LoginFail" << endl;
//			udb.DisconnectDataSource();
//			clients[key]->DoSendLoginPacket(false);
//			return;
//		}
//	}
//	cout << "client[" << clients[key]->_cid << "] " << "LoginSuccess" << endl;
//	udb.DisconnectDataSource();
//	clients[key]->DoSendLoginPacket(true);
//}


//void ProcessPacket(int32 key, char* packet)
//{
//	switch (packet[1])
//		{
//		case (int8)S_PACKET_TYPE::LOGIN_OK:
//		{
//
//			S2C_LOGIN_OK* lp = (S2C_LOGIN_OK*)packet;
//			_cid = lp->cid;
//		}
//		break;
//		case (int8)S_PACKET_TYPE::LOGIN_FAIL:
//		{
//			cout << "Login fail" << endl;
//			closesocket(_sock);
//		}
//		break;
//		case (int8)S_PACKET_TYPE::CHAT:
//		{
//			_CHAT* chat = reinterpret_cast<_CHAT*>(packet);
//			std::cout << chat->buf << std::endl;
//		}
//		break;
//		case (int8)C_PACKET_TYPE::CHAT:
//		{
//
//			_CHAT* cp = (_CHAT*)packet;
//			std::cout << "client[" << _cid << "] 's msg : " << cp->buf << endl;
//		}
//		break;
//		case (int8)C_PACKET_TYPE::ACQ_LOGIN:
//		{
//
//			C2S_LOGIN* cp = (C2S_LOGIN*)packet;
//			wstring sqlExec(L"EXEC search_user_db ");
//			sqlExec += cp->name;
//			sqlExec += L", ";
//			sqlExec += cp->pw;
//			DB_Worker(key, sqlExec);
//		}
//	break;
//	}
//}

