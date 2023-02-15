#include "pch.h"
#include "ThreadManager.h"
#include "SocketUtil.h"
#include "AcceptManager.h"
int main()
{
	::SetConsoleTitle(L"SERVER");
	AcceptManager listener;
	listener.InitAccept();

	for (int32 i = 0; i < thread::hardware_concurrency() - 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					ServerIocpCore.Processing(); // Accept 받기 성공 
					//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
					std::this_thread::sleep_for(0ms);
				}
			});
	}
	GThreadManager->Launch([=]()
		{
			while (true)
			{
				ServerIocpCore._rmgr->UpdateRooms();
				std::this_thread::sleep_for(0ms);
			}
		});
	GThreadManager->Join();

}