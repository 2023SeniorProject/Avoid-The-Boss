#include "pch.h"
#include "RoomManager.h"
#include "Session.h"
#include "packetEvent.h"

using namespace std;
//========== ROOM =============

//    cList Lock 호출 타이밍
//    1. 쓰는 경우 : 유저가 방에 들어오거나 나갈 때, cList 값을 갱신할 때
//    2. 읽는 경우 : 방에 있는 유저에게 브로드 캐스팅 진행하는 경우 cList를 탐색할 때

//=============================

void Room::UserOut(int32 sid)
{
	{
		//WLock; // cList Lock 쓰기 호출
		std::unique_lock<std::shared_mutex> wll(_listLock);
		auto i = std::find(_cList.begin(), _cList.end(), sid); // 리스트에 있는지 탐색 후
		if (i != _cList.end()) _cList.erase(i); // 리스트에서 제거
	}

	if (IsDestroyRoom())
	{
		_status = ROOM_STATUS::EMPTY;
		S2C_HIDE_ROOM packet;
		packet.size = sizeof(S2C_HIDE_ROOM);
		packet.rmNum = _num;
		// 업데이트 리스트를 보내준다. ==> 빈방이므로 더 이상 표시 X
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

	// 비어있는 방 (만들어지지 않은 방) or 현재 인원수가 4명이면 접속 실패 
	if (_cList.size() >= MAX_ROOM_USER || _status == ROOM_STATUS::EMPTY)
	{
		// enter fail
		packet.success = 0;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if(_cList.size() < MAX_ROOM_USER && _status != ROOM_STATUS::EMPTY) // 아니면 접속 성공
	{
		packet.success = 1;
		{
			//WLock; // cList Lock 쓰기 호출 
			std::unique_lock<std::shared_mutex> wll(_listLock);
			_cList.push_back(sid);
			if (_cList.size() == 4) _status = ROOM_STATUS::FULL;
		}
			
		ServerIocpCore._clients[sid]->_myRm = _num;
		ServerIocpCore._clients[sid]->_status = USER_STATUS::ROOM;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		
	}
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
	// 갱신하는걸 보내줄지 말지 미정
}

void Room::BroadCasting(void* packet) // 방에 속하는 클라이언트에게만 전달하기
{
	//RLock; // cList Lock 읽기 호출 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto i =  _cList.begin(); i != _cList.end(); ++i)
	{
		if (ServerIocpCore._clients[*i] == nullptr)
		{
			continue;
		}
		if(!ServerIocpCore._clients[*i]->DoSend(packet))
		{ 
			// 비정상 접속 클라이언트 처리
			cout << *i << "Client Error Occur" << endl;
			continue;
		}
	}
}

void Room::Update()
{
	//if(!(_rmTimer._nWorldFrame % 10))
	//{
	//
	//	S2C_POSITION packet;
	//	std::shared_lock<std::shared_mutex> rlock(_listLock);
	//	for (auto i : _cList)
	//	{
	//		packet.sid = i;
	//		packet.size = sizeof(S2C_POSITION);
	//		packet.type = S_PACKET_TYPE::POSITION;
	//		ServerIocpCore._clients[i]->_playerLock.lock();
	//		packet.position = ServerIocpCore._clients[i]->_playerInfo.GetPosition();
	//		ServerIocpCore._clients[i]->_playerLock.unlock();
	//		//ServerIocpCore._clients[i]->DoSend(&packet);	
	//	}
	//}
	_rmTimer.Tick(60.f);
	

	{
		std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue WLock 호출
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
	//RLock; // cList Lock 읽기 호출
	
	std::shared_lock<std::shared_mutex> rll(_listLock);
	
	for (auto i = _cList.begin(); i != _cList.end(); ++i)
	{
		if (ServerIocpCore._clients[*i] == nullptr)
		{
			continue;
		}
		std::lock_guard<std::mutex> pl(ServerIocpCore._clients[*i]->_playerLock);
		ServerIocpCore._clients[*i]->_playerInfo.Update(_rmTimer.GetTimeElapsed());
	}
}

void Room::AddEvent(queueEvent* qe)
{
	std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue Lock 호출
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
		if (_rooms[i]._status == ROOM_STATUS::EMPTY) continue;
		_rooms[i].Update();
	}
}
void RoomManager::ExitRoom(int32 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}