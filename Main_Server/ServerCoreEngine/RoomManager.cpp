#include "pch.h"
#include "RoomManager.h"
#include "Session.h"

//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	
	auto i = std::find(_cList.begin(),_cList.end(), sid); // 리스트에 있는지 탐색 후
	if(i != _cList.end()) _cList.erase(i); // 리스트에서 제거
}

void Room::UserIn(int16 sid)
{
	if (_cList.size() == 0) _hId = sid; // 처음 들어온 유저 (방 생성 유저)라면 자신이 호스트 세션이 된다.
	_cList.push_back(sid);
}

void Room::BroadCasting(GameSession& c)
{
	READ_LOCK;
	for (auto i : _cList)
	{

	}
}
// ======= RoomManager ========

// ============================

bool RoomManager::CreateRoom(int16 hid)
{
	_rooms[_rmCnt].ChangeStatus(NOT_FULL);
	_rooms[_rmCnt].UserIn(hid);
	return true;
}

void RoomManager::RemoveRoom(int16 rm_id) 
{

	
}
void RoomManager::EnterRoom(int16 sid,int16 rmNum)
{

}
void RoomManager::ExitRoom(int16 sid, int16 rmNum)
{

}