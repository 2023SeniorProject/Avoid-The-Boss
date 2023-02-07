#include "pch.h"
#include "RoomManager.h"
#include "Session.h"

//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	auto i = std::find(_cList.begin(),_cList.end(), sid); // ����Ʈ�� �ִ��� Ž�� ��
	if(i != _cList.end()) _cList.erase(i); // ����Ʈ���� ����
	if (!_cList.size())
	{
		_status = ROOM_STATUS::EMPTY;
		S2C_HIDE_ROOM packet;
		packet.size = sizeof(S2C_HIDE_ROOM);
		packet.rmNum = _num;
		// ������Ʈ ����Ʈ�� �����ش�. ==> ����̹Ƿ� �� �̻� ǥ�� X
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

	// ���� �ο����� 4���̸� ���� ���� 
	if (_cList.size() >= 4)
	{
		// enter fail
		packet.success = 0;
		GIocpCore._clients[sid]->DoSend(&packet);
	}
	else  // �ƴϸ� ���� ����
	{
		packet.success = 1;
		_cList.push_back(sid);
		if (_cList.size() == 4) _status = ROOM_STATUS::FULL;
		GIocpCore._clients[sid]->DoSend(&packet);
	}
	// �����ϴ°� �������� ���� ����
}

template<class T>
void Room::BroadCasting(T packet) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
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