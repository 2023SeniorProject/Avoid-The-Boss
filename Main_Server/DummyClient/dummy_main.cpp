#include "pch.h"
#include "SocketUtil.h"
#include "ThreadManager.h"
#include "iocpCore.h"
#include "ClientManager.h"
#include "Session.h"



ClientManager cmgr;


void SendThread()
{
	
	char sendBuffer[100];
	while (true)
	{
		switch (cmgr._clientSession._curScene)
		{
		case 0:
			{
				int32 menu;
				std::cout << "명령어(1: 방 생성 2: 방 입장): ";
				std::cin >> menu;
				
				switch (menu)
				{
					case 1:
					{
						C2S_ROOM_CREATE packet;
						packet.size = sizeof(C2S_ROOM_CREATE);
						packet.type = C_ROOM_PACKET_TYPE::ACQ_MK_RM;
						cmgr.DoSend(&packet);
						break;
					}
					case 2:
					{
						uint16 num;
						std::cout << std::endl;
						std::cout << "방 번호 : ";
						cin >> num;
						C2S_ROOM_ENTER packet;
						packet.size = sizeof(C2S_ROOM_ENTER);
						packet.type = C_ROOM_PACKET_TYPE::ACQ_ENTER_RM;
						packet.rmNum = num;
						cmgr.DoSend(&packet);
						break;
					}
				}

				break;
			}
		case 1:
			{
				std::cin.getline(sendBuffer, 100);
				_CHAT chat_packet;
				chat_packet.cid = (uint8)cmgr._clientSession._cid;
				chat_packet.size = sizeof(_CHAT);
				chat_packet.type = (uint8)C_PACKET_TYPE::CCHAT;
				strcpy_s(chat_packet.buf, CHATBUF, sendBuffer);
				chat_packet.buf[CHATBUF - 1] = '\0';
				cmgr.DoSend(&chat_packet);
				break;
			}
		case 2:
			{
				break;
			}

		}
		if (cmgr._clientSession._sock == INVALID_SOCKET) break;
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
	cmgr.InitConnect("127.0.0.1");
	cmgr.DoConnect(reinterpret_cast<char*>(&loginPacket));
	
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