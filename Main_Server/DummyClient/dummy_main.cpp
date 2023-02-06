#include "pch.h"
#include "SocketUtil.h"
#include "ThreadManager.h"
#include "iocpCore.h"
#include "ClientManager.h"
#include "Session.h"


void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}



ClientManager cmgr;

int32 ClientManager::_scene = 0;

void SendThread()
{
	char sendBuffer[100];
	while (true)
	{
		if (cmgr._clientSession._status.load() == STATUS::EMPTY) continue;
		std::cout << "msg : ";
		std::cin.getline(sendBuffer, 100);

		_CHAT chat_packet;
		chat_packet.size = sizeof(_CHAT);
		chat_packet.type = (uint8)C_PACKET_TYPE::CCHAT;
		strcpy_s(chat_packet.buf, 99, sendBuffer);
		cmgr.DoSend(&chat_packet);
	}

}

void SceneThread()
{
	switch (ClientManager::_scene)
	{
		// 로비
	case 0:
		break;

		// 방
	case 1:
		break;
		// 게임
	case 3:
		break;
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

	cmgr.InitConnect("127.0.0.1");
	cmgr.DoConnect(reinterpret_cast<char*>(&loginPacket));
	
	GCThreadManager->Launch([=]()
		{
			while (true)
			{
				ClientIocpCore.Processing(); 
				//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
			}
		});
	GCThreadManager->Launch(SendThread);
	GCThreadManager->Join();
	SocketUtil::Clear();
}