#include "pch.h"
#include "RoomManager.h"
#include "Session.h"

//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	auto i = std::find(_cList.begin(),_cList.end(), sid); // 리스트에 있는지 탐색 후
	if(i != _cList.end()) _cList.erase(i); // 리스트에서 제거
	if (!_cList.size())
	{
		_status = ROOM_STATUS::EMPTY;
		S2C_HIDE_ROOM packet;
		packet.size = sizeof(S2C_HIDE_ROOM);
		packet.rmNum = _num;
		// 업데이트 리스트를 보내준다. ==> 빈방이므로 더 이상 표시 X
		for (auto i : GIocpCore._clients)
		{
			i->DoSend(&packet);
		}
	}
}

void Room::UserIn(int16 sid)
{

	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = S_ROOM_PACKET_TYPE::ENTER_SRM;

	// 현재 인원수가 4명이면 접속 실패 
	if (_cList.size() >= 4)
	{
		// enter fail
		packet.success = 0;
		GIocpCore._clients[sid]->DoSend(&packet);
	}
	else  // 아니면 접속 성공
	{
		packet.success = 1;
		_cList.push_back(sid);
		if (_cList.size() == 4) _status = ROOM_STATUS::FULL;
		GIocpCore._clients[sid]->DoSend(&packet);
	}
	// 갱신하는걸 보내줄지 말지 미정
}

template<class T>
void Room::BroadCasting(T packet) // 방에 속하는 클라이언트에게만 전달하기
{
	for (auto i : _cList)
	{
		GIocpCore._clients[*i]->DoSend(&packet);
	}
}
// ======= RoomManager ========

// ============================
void RoomManager::Init()
{
	for (int i = 0; i < _cap; ++i) _rooms[i]._num = i;
}

void RoomManager::EnterRoom(int16 sid,int16 rmNum)
{
	_rooms[rmNum].UserIn(sid);
}

void RoomManager::CreateRoom(int16 sid)
{
	for (int i = 0; i < 100; ++i)
	{
		if (_rooms[i]._status == ROOM_STATUS::EMPTY)
		{
			_rooms[i].UserIn(sid);
			_rooms[i]._status = ROOM_STATUS::NOT_FULL;
			break;
		}
	}
}

void RoomManager::ExitRoom(int16 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}