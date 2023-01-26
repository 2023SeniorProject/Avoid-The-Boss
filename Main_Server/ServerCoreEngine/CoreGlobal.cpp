#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();

}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}