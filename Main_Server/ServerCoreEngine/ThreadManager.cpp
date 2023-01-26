#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
// function --> void 인풋의 void 반환 함수를 받아준다.
// 외에 여러가지 함수 포인터를 받을 수 있음.
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
	lThreadId = sThreadId.fetch_add(1); // 스레드 id 부여
}

void ThreadManager::DestroyTLS()
{
}
