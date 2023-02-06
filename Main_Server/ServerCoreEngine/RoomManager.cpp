#include "pch.h"
#include "RoomManager.h"


//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	auto i = std::find(_cList.begin(),_cList.end(), sid);
	_cList.erase(i); // ����Ʈ���� ����
}

void Room::UserIn(int16 sid)
{
	_cList.push_back(sid);
}
// ======= RoomManager ========

// ============================

bool RoomManager::CreateRoom(int16 hid)
{
	if (_cap <= _rmCnt) return false; // �ִ� �� �������� �Ѿ ��� ���� X
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
