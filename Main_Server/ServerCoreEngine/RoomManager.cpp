#include "pch.h"
#include "RoomManager.h"
#include "Session.h"

//========== ROOM =============

//=============================

void Room::UserOut(int16 sid)
{
	{
		WLock;
		auto i = std::find(_cList.begin(), _cList.end(), sid); // ����Ʈ�� �ִ��� Ž�� ��
		if (i != _cList.end()) _cList.erase(i); // ����Ʈ���� ����	
	}

	if (!_cList.size())
	{
		_status = ROOM_STATUS::EMPTY;
		S2C_HIDE_ROOM packet;
		packet.size = sizeof(S2C_HIDE_ROOM);
		packet.rmNum = _num;
		// ������Ʈ ����Ʈ�� �����ش�. ==> ����̹Ƿ� �� �̻� ǥ�� X
		{
			READ_IOCP_LOCK;
			for (auto i : GIocpCore._clients)
			{
				i.second->DoSend(&packet);
			}
		}
	}
}

void Room::UserIn(int16 sid)
{

	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = S_ROOM_PACKET_TYPE::REP_ENTER_RM;

	// ����ִ� �� (��������� ���� ��) or ���� �ο����� 4���̸� ���� ���� 
	if (_cList.size() >= 4 || _status == ROOM_STATUS::EMPTY)
	{
		// enter fail
		packet.success = 0;
		GIocpCore._clients[sid]->DoSend(&packet);
	}
	else if(_cList.size() < 4 && _status != ROOM_STATUS::EMPTY) // �ƴϸ� ���� ����
	{
		packet.success = 1;
		{
			WLock;
			_cList.push_back(sid);
		}
		if (_cList.size() == 4) _status = ROOM_STATUS::FULL;
		
		GIocpCore._clients[sid]->_myRm = _num;
		GIocpCore._clients[sid]->_status = USER_STATUS::ROOM;
		GIocpCore._clients[sid]->DoSend(&packet);
		
	}
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
	// �����ϴ°� �������� ���� ����
}


void Room::BroadCasting(void* packet) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
{
	RLock;
	for (auto i : _cList)
	{
		GIocpCore._clients[i]->DoSend(packet);
	}
}
// ======= RoomManager ========

// ============================
void RoomManager::Init()
{
	for (int i = 0; i < _cap; ++i)
	{
		_rooms[i]._cList.clear();
		_rooms[i]._num = i;
	}
}

void RoomManager::EnterRoom(int16 sid,int16 rmNum)
{
	_rooms[rmNum].UserIn(sid);
}

void RoomManager::CreateRoom(int16 sid)
{
	if (_rmCnt.load() >= _cap)
	{
		S2C_ROOM_CREATE packet;
		packet.size = sizeof(S2C_ROOM_CREATE);
		packet.type = S_ROOM_PACKET_TYPE::MK_RM_FAIL;
		GIocpCore._clients[sid]->DoSend(&packet);
		return;
	}
	for (int i = 0; i < 100; ++i)
	{
		if (_rooms[i]._status == ROOM_STATUS::EMPTY)
		{
			_rooms[i]._status = ROOM_STATUS::NOT_FULL;
			_rooms[i].UserIn(sid);
			_rmCnt.fetch_add(1);
			break;
		}
	}
}

void RoomManager::ExitRoom(int16 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}