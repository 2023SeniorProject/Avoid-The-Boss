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

}