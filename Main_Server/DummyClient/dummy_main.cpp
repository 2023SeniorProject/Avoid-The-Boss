#include "pch.h"
#include "SocketUtil.h"
#include "ThreadManager.h"
#include "iocpCore.h"
#include "ClientManager.h"
#include "Session.h"



ClientManager cmgr;

int32 ClientManager::_scene = 0;

void SendThread()
{
	
	char sendBuffer[100];
	while (true)
	{
	
		if (cmgr._clientSession._status.load() == STATUS::EMPTY) continue;
		std::cin.getline(sendBuffer, 100);
		cout << endl;
		_CHAT chat_packet;
		chat_packet.sid = (uint8)cmgr._clientSession._cid;
		chat_packet.size = sizeof(_CHAT);
		chat_packet.type = (uint8)C_PACKET_TYPE::CCHAT;
		strcpy_s(chat_packet.buf, CHATBUF, sendBuffer);
		chat_packet.buf[CHATBUF - 1] = '\0';
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
	::system("cls");
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