#include "pch.h"
#include "RoomManager.h"
#include "Session.h"
#include "packetEvent.h"

using namespace std;
//========== ROOM =============

//    cList Lock ȣ�� Ÿ�̹�
//    1. ���� ��� : ������ �濡 �����ų� ���� ��, cList ���� ������ ��
//    2. �д� ��� : �濡 �ִ� �������� ��ε� ĳ���� �����ϴ� ��� cList�� Ž���� ��

//=============================

void Room::UserOut(int32 sid)
{
	{
		//WLock; // cList Lock ���� ȣ��
		std::unique_lock<std::shared_mutex> wll(_listLock);
		auto i = std::find(_cList.begin(), _cList.end(), sid); // ����Ʈ�� �ִ��� Ž�� ��
		if (i != _cList.end()) _cList.erase(i); // ����Ʈ���� ����
	}

	if (IsDestroyRoom())
	{
		_status = ROOM_STATUS::EMPTY;
		S2C_HIDE_ROOM packet;
		packet.size = sizeof(S2C_HIDE_ROOM);
		packet.rmNum = _num;
		// ������Ʈ ����Ʈ�� �����ش�. ==> ����̹Ƿ� �� �̻� ǥ�� X
		{
			READ_SERVER_LOCK;
			for (auto i : ServerIocpCore._clients)
			{
				i.second->DoSend(&packet);
			}
		}
	}
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
}

void Room::UserIn(int32 sid)
{

	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = S_ROOM_PACKET_TYPE::REP_ENTER_RM;

	// ����ִ� �� (��������� ���� ��) or ���� �ο����� 4���̸� ���� ���� 
	if (_cList.size() >= MAX_ROOM_USER || _status == ROOM_STATUS::EMPTY)
	{
		// enter fail
		packet.success = 0;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if (_cList.size() < MAX_ROOM_USER && _status != ROOM_STATUS::EMPTY) // �ƴϸ� ���� ����
	{
		packet.success = 1;
		ServerIocpCore._clients[sid]->_myRm = _num;
		ServerIocpCore._clients[sid]->_status = USER_STATUS::ROOM;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		{
			//cList Lock ���� ȣ�� 
			std::unique_lock<std::shared_mutex> wll(_listLock);
			_cList.push_back(sid);
		}
		if (_cList.size() == PLAYERNUM)
		{
			_status = ROOM_STATUS::FULL;
			S2C_GAMESTART packet;
			packet.type = S_PACKET_TYPE::GAME_START;
			packet.size = sizeof(S2C_GAMESTART);
			int k = 0;
			for (auto i : _cList)
			{
				packet.sids[k] = i;
				++k;
			}
			BroadCasting(&packet);
			_logic.StartGame();
		}
	}		
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
	// �����ϴ°� �������� ���� ����
}

void Room::BroadCasting(void* packet) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
{
	//RLock; // cList Lock �б� ȣ�� 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto i =  _cList.begin(); i != _cList.end(); ++i)
	{
		if (ServerIocpCore._clients[*i] == nullptr) continue;
		if(!ServerIocpCore._clients[*i]->DoSend(packet))
		{ 
			// ������ ���� Ŭ���̾�Ʈ ó��
			cout << *i << "Client Error Occur" << endl;
			continue;
		}
	}
}

// �濡 �ִ� ������ ���� ���� ���� ������Ʈ ����
void Room::Update()
{
	if (_status != ROOM_STATUS::FULL) return;

	{
		std::unique_lock<std::shared_mutex> ql(_jobQueueLock);
		while(!_jobQueue.empty())
		{
			queueEvent* qe = _jobQueue.front();
			_jobQueue.pop();
			if (qe != nullptr)
			{
				qe->Task();
				delete qe;
			}
		}
	}	


	_logic.UpdateWorld(60.f, _players);
}

void Room::AddEvent(queueEvent* qe)
{
	std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue Lock ȣ��
	_jobQueue.push(qe);
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

void RoomManager::EnterRoom(int32 sid,int16 rmNum)
{
	_rooms[rmNum].UserIn(sid);
}

void RoomManager::CreateRoom(int32 sid)
{
	if (_rmCnt.load() >= _cap)
	{
		S2C_ROOM_CREATE packet;
		packet.size = sizeof(S2C_ROOM_CREATE);
		packet.type = S_ROOM_PACKET_TYPE::MK_RM_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		return;
	}
	for (int i = 0; i < 100; ++i)
	{
		if (_rooms[i]._status == ROOM_STATUS::EMPTY)
		{
			_rooms[i]._status = ROOM_STATUS::NOT_FULL;
			_rooms[i]._rmTimer.Reset();
			
			_rooms[i].UserIn(sid);
			_rmCnt.fetch_add(1);
			break;
		}
	}
}

void RoomManager::UpdateRooms()
{
	for (int i = 0; i < _cap; ++i)
	{
		if (_rooms[i]._status != ROOM_STATUS::FULL) continue;
		_rooms[i].Update();
	}
}
void RoomManager::ExitRoom(int32 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}