#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtil.h"

ThreadManager* GThreadManager = nullptr;
ThreadManager* GCThreadManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal() 
	{
		GThreadManager = new ThreadManager();
		GCThreadManager = new ThreadManager();
		SocketUtil::Init();

	};
	~CoreGlobal() 
	{
		delete GThreadManager;
		delete GCThreadManager;
		SocketUtil::Clear();
	};
}GCoreGlobal;