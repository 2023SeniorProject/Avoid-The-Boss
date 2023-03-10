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
		
		SocketUtil::Init();

	};
	~CoreGlobal() 
	{
	
		SocketUtil::Clear();
	};
}GCoreGlobal;