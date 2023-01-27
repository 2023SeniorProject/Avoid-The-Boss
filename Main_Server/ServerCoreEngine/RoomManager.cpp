#include "pch.h"
#include "RoomManager.h"

bool RoomManager::CreateRoom(const int16& hid)
{
	if (_capacity >= _rm_cnt) return false; // 최대 방 개수보다 넘어갈 경우 생성 X
	_rooms.try_emplace(_rm_cnt++, new Room(hid));
	return true;
}

void RoomManager::RemoveRoom(const int16& rm_id) 
{
	_rooms[rm_id]->DestroyRoom();
}
