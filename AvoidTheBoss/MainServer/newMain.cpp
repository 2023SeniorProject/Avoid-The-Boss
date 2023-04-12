#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "pch.h"
#include "ThreadManager.h"
#include "SocketUtil.h"
#include "AcceptManager.h"
#include "CollisionDetector.h"


int main()
{
	std::ios::sync_with_stdio(false);
	std::cout.tie(NULL);
	::SetConsoleTitle(L"SERVER");
	SocketUtil::Init();
	
	ThreadManager* GThreadManager = nullptr;
	GThreadManager = new ThreadManager;

	AcceptManager listener;
	listener.InitAccept();
	GThreadManager = new ThreadManager;
	for (int32 i = 0; i < thread::hardware_concurrency() - 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					ServerIocpCore.Processing(); // Accept 받기 성공 
					//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
				}
				std::cout << "End Thread \n";
				_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
				_CrtDumpMemoryLeaks();
			});
		
	}
	GThreadManager->Launch([=]()
		{
			while (true)
			{
				ServerIocpCore._rmgr->UpdateRooms();
			}
			std::cout << "End Thread \n";
		});
	GThreadManager->Join();
	delete GThreadManager;
	SocketUtil::Clear();

	
}