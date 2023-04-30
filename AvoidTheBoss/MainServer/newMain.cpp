
#include "pch.h"
#include "ThreadManager.h"
#include "SocketUtil.h"
#include "AcceptManager.h"
#include "CSIocpCore.h"

int main()
{
	std::ios::sync_with_stdio(false);
	std::cout.tie(NULL);
	::SetConsoleTitle(TEXT("GAMESERVER"));
	SocketUtil::Init();
	
	ThreadManager* GThreadManager = nullptr;
	GThreadManager = new ThreadManager;

	AcceptManager listener;
	listener.InitAccept();
	GThreadManager = new ThreadManager;
	for (int32 i = 0; i < thread::hardware_concurrency() - 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					
					ServerIocpCore.Processing(); // Accept 받기 성공 
					//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
					std::this_thread::sleep_for(0ms);
				}
				std::cout << "End Thread \n";
			});
		
	}
	GThreadManager->Launch([=]()
		{
			while (true)
			{
				ServerIocpCore._rmgr->UpdateRooms();
				std::this_thread::sleep_for(0ms);
			}
			std::cout << "End Thread \n";
		});
	GThreadManager->Join();
	delete GThreadManager;
	SocketUtil::Clear();

	
}