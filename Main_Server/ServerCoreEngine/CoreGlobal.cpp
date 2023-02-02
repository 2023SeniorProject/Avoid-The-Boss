#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtil.h"

ThreadManager* GThreadManager = nullptr;


class CoreGlobal
{
public:
	CoreGlobal() 
	{
		GThreadManager = new ThreadManager();
		SocketUtil::Init();

	};
	~CoreGlobal() 
	{
		delete GThreadManager;
		SocketUtil::Clear();
	};
}GCoreGlobal;