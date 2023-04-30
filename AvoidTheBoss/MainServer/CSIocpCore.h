#pragma once
#include "IocpCore.h"
#include "RoomManager.h"

class CSIocpCore : public IocpCore
{

public:
	CSIocpCore();
	~CSIocpCore();
	virtual void Disconnect(int32 sid) override;
public:
	std::shared_mutex _lock;
	std::unordered_map<int32, ServerSession*> _clients;
	std::set<int32> _cList;
	RoomManager* _rmgr;
};


extern CSIocpCore ServerIocpCore;