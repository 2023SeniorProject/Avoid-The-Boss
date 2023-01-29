#pragma once
#include <thread>
#include <functional>

using namespace std;
class ThreadManager
{
public:
	ThreadManager() { InitTLS(); }
	~ThreadManager() { Join(); }

	
	void Launch(function<void(HANDLE)> callback, HANDLE);
	void Join();
	//TLS init
	static void InitTLS();
	static void DestroyTLS();
private:
	Mutex _m;
	vector<thread> _threads;
};



