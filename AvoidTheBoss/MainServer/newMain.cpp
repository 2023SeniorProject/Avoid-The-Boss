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
					ServerIocpCore.Processing(); // Accept �ޱ� ���� 
					//���� ���� ���� ���α׷��� Worker Thread�� �ش��ϴ� �κ�
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
	delete GThreadManager;
	SocketUtil::Clear();
}