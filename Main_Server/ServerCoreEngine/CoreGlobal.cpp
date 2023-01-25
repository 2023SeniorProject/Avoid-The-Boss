#include "pch.h"
#include "CoreGlobal.h"

ThreadManager* GThreadManager = nullptr;
CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();

}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}