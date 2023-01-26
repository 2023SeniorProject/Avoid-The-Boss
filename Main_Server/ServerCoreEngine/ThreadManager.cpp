#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
// function --> void ��ǲ�� void ��ȯ �Լ��� �޾��ش�.
// �ܿ� �������� �Լ� �����͸� ���� �� ����.
void ThreadManager::Launch(function<void(void)> callback)
{
	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));

}

void ThreadManager::Join()
{
	for (auto& i : _threads)
	{
		if (i.joinable()) i.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> sThreadId = 0;
	lThreadId = sThreadId.fetch_add(1); // ������ id �ο�
}

void ThreadManager::DestroyTLS()
{
}
