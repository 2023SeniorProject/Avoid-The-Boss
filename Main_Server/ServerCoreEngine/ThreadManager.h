#pragma once
#include <thread>
#include <functional>
class ThreadManager
{
public:
	ThreadManager() { InitTLS(); }
	~ThreadManager() { Join(); }

	void Launch(function<void(void)> callback);
	void Join();
	//TLS init
	static void InitTLS();
	static void DestroyTLS();
private:
	Mutex _m;
	vector<thread> _threads;
	
};

