#include "pch.h"
#include "RoomManager.h"


//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	auto i = std::find(_cList.begin(),_cList.end(), sid);
	_cList.erase(i); // 리스트에서 제거
}

void Room::UserIn(int16 sid)
{
	_cList.push_back(sid);
}
// ======= RoomManager ========

// ============================

bool RoomManager::CreateRoom(int16 hid)
{
	if (_cap <= _rmCnt) return false; // 최대 방 개수보다 넘어갈 경우 생성 X
	_rooms.try_emplace(_rmCnt++, Room(hid));
	return true;
}

void RoomManager::RemoveRoom(int16 rm_id) 
{
	if (_rooms[rm_id].IsDestroyRoom())
	{
		_rooms.erase(rm_id);
	}
	
}
