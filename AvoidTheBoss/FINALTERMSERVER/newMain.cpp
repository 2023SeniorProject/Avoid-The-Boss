#include "pch.h"
#include "ThreadManager.h"
#include "SocketUtil.h"
#include "AcceptManager.h"
int main()
{
	AcceptManager listener;
	listener.InitAccept();

	for (int32 i = 0; i < thread::hardware_concurrency(); i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					ServerIocpCore.Processing(); // Accept �ޱ� ���� 
					//���� ���� ���� ���α׷��� Worker Thread�� �ش��ϴ� �κ�
				}
			});
	}

	GThreadManager->Join();

}