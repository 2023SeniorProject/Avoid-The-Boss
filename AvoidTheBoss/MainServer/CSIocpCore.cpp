#include "pch.h"
#include "CSIocpCore.h"
#include "Session.h"
#include "CollisionDetector.h"

CSIocpCore ServerIocpCore;

CSIocpCore::CSIocpCore()
{
	_rmgr = new RoomManager();
	_rmgr->Init();
	BoxTree = new OcTree(XMFLOAT3(0, 0, 0), 60);
	BoxTree->BuildTree();
	BoxTree->ReadBoundingBoxInfoFromFile("bounding_boxes.txt");
}

CSIocpCore::~CSIocpCore()
{
	delete _rmgr;
}


void CSIocpCore::Disconnect(int32 sid)
{
	std::cout << "[" << _clients[sid]->_sid << "] Disconnected" << std::endl;
	if(sid >= 0 && _clients[sid]->_myRm != -1) _rmgr->ExitRoom(sid, _clients[sid]->_myRm);
	
	{
		WRITE_SERVER_LOCK;
		_cList.erase(_clients[sid]->_sid);
		_clients.erase(sid);
		std::cout << "Dead Lock Checking\n";
	}
}