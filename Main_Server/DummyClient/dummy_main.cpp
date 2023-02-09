#include "pch.h"
#include "SocketUtil.h"
#include "ThreadManager.h"
#include "iocpCore.h"
#include "ClientManager.h"
#include "Session.h"



void SendThread()
{
	
	char sendBuffer[100];
	Atomic<bool> IsErrorOccur = true;
	while (IsErrorOccur)
	{
		if (ClientIocpCore._client->_status == USER_STATUS::EMPTY) continue;
		if (ClientIocpCore._client->_curScene == 0)
		{
			int32 menu;
			std::cout << "명령어(1: 방 생성 2: 방 입장): ";
			std::cin >> menu;
			std::cin.ignore();
			std::cout << std::endl;
			switch (menu)
			{
				case 1:
				{
					C2S_ROOM_CREATE packet;
					packet.size = sizeof(C2S_ROOM_CREATE);
					packet.type = C_ROOM_PACKET_TYPE::ACQ_MK_RM;
					if (!ClientIocpCore._client->DoSend(&packet))
					{
						IsErrorOccur.store(false);
					}
					break;
				}
				case 2:
				{
					int32 num;
					std::cout << "방 번호 : ";
					cin >> num;
					cin.ignore();
					C2S_ROOM_ENTER packet;
					packet.size = sizeof(C2S_ROOM_ENTER);
					packet.type = C_ROOM_PACKET_TYPE::ACQ_ENTER_RM;
					packet.rmNum = num;
					if (!ClientIocpCore._client->DoSend(&packet))
					{
						IsErrorOccur.store(false);
					}
					break;
				}
			}
			::system("cls");
		}
		else if(ClientIocpCore._client->_curScene > 0)
		{
			std::cin.getline(sendBuffer, 100);
			_CHAT chat_packet;
			chat_packet.cid = (uint8)ClientIocpCore._client->_cid;
			chat_packet.size = sizeof(_CHAT);
			chat_packet.type = (uint8)C_PACKET_TYPE::CCHAT;
			strcpy_s(chat_packet.buf, CHATBUF, sendBuffer);
			chat_packet.buf[CHATBUF - 1] = '\0';
			if(!ClientIocpCore._client->DoSend(&chat_packet)) break;
		}
	}
}

int main()
{
#pragma region init winsock
	SocketUtil::Init();
	
	
	C2S_LOGIN loginPacket;
	loginPacket.size = sizeof(C2S_LOGIN);
	loginPacket.type = (uint8)C_PACKET_TYPE::ACQ_LOGIN;
	std::cout << "ID : ";
	std::wcin.getline(loginPacket.name, sizeof(WCHAR) * 10);
	std::cout << "PW : ";
	std::wcin.getline(loginPacket.pw, sizeof(WCHAR) * 10);
	::system("cls");
	
	ClientIocpCore.InitConnect("127.0.0.1");
	ClientIocpCore.DoConnect(reinterpret_cast<char*>(&loginPacket));

	GCThreadManager->Launch([=]()
		{
			while (true)
			{
				if(!ClientIocpCore.Processing()) break; 
				//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
			}
		});
	GCThreadManager->Launch(SendThread);
	GCThreadManager->Join();
	SocketUtil::Clear();
}