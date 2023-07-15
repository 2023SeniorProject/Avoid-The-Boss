#include "pch.h"
#include "RoomManager.h"
#include "CSIocpCore.h"
#include "JobQueue.h"


using namespace std;
//========== ROOM =============

//    cList Lock 호출 타이밍
//    1. 쓰는 경우 : 유저가 방에 들어오거나 나갈 때, cList 값을 갱신할 때
//    2. 읽는 경우 : 방에 있는 유저에게 브로드 캐스팅 진행하는 경우 cList를 탐색할 때

//=============================
Room::Room()
{
}

Room::~Room()
{
}

void Room::UserOut(int32 sid)
{
	int idx = 0;
	{
		// cList Lock 쓰기 호출
		_gameLogic.GetPlayerBySid(sid).SetVelocity(XMFLOAT3(0, 0, 0)); // 속도 0
		idx = _gameLogic.GetPlayerBySid(sid).m_idx; /// 인덱스 가져오기
		_gameLogic.GetPlayerBySid(sid).m_hide = true; // 업데이트 false로 변경
		
		std::unique_lock<std::shared_mutex> wll(_listLock);
		auto i = std::find(_cList.begin(), _cList.end(), sid); // 리스트에 있는지 탐색 후
		if (i != _cList.end()) _cList.erase(i); // 리스트에서 제거
		_mem.fetch_sub(1);
		S2C_ROOM rmpacket;
		rmpacket.size = sizeof(S2C_ROOM);
		rmpacket.type = (int8)S_ROOM_PACKET_TYPE::UPDATE_LIST;
		rmpacket.member = _mem.load();
		rmpacket.rmNum = _num;
	
		{
			READ_SERVER_LOCK;
			ServerIocpCore.BroadCastingAll(&rmpacket);
		}
	}
		
	std::cout << "LEFT USER SID LIST [";
	for (auto i : _cList) std::cout << i << ", ";
	std::cout << " ]\n";
	
	// 나간 플레이어는 숨기도록 한다.
	SC_EVENTPACKET packet;
	packet.size = sizeof(SC_EVENTPACKET);
	packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
	packet.eventId = (uint8)EVENT_TYPE::HIDE_PLAYER_ONE + idx;
	BroadCasting(&packet);
	if (IsDestroyRoom())
	{
		_status = (uint8)ROOM_STATUS::EMPTY;
		
		std::cout << "Destroy Room\n";
	}
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
}

void Room::UserIn(int32 sid)
{

	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_RM;

	// 비어있는 방 (만들어지지 않은 방) or 현재 인원수가 4명이면 접속 실패 
	if (_cList.size() >= MAX_ROOM_USER || _status == (int8)ROOM_STATUS::EMPTY)
	{
		// enter fail
		packet.success = 0;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if (_cList.size() < MAX_ROOM_USER && _status != (int8)ROOM_STATUS::EMPTY) // 아니면 접속 성공
	{
		packet.success = 1;
		ServerIocpCore._clients[sid]->_myRm = _num;
		ServerIocpCore._clients[sid]->_status = USER_STATUS::ROOM;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		
		{
			//cList Lock 쓰기 호출 
			std::unique_lock<std::shared_mutex> wll(_listLock);
			_cList.push_back(sid);
		}

		_mem.fetch_add(1);

	/*	if (_cList.size() == PLAYERNUM)
		{
			
			S2C_GAMESTART packet;
			packet.type = (uint8)S_PACKET_TYPE::GAME_START;
			packet.size = sizeof(S2C_GAMESTART);
			int k = 0;
			for (auto i : _cList)
			{
				packet.sids[k] = i;
				_gameLogic.SetPlayerSidAndIdx(i, k);
				++k;
			}
			std::cout << "GAME START\n";
			std::cout << "TOTAL USER SID LIST[";
			for (auto i : _cList) std::cout << i << " | ";
			std::cout << " ]\n";
			BroadCasting(&packet);
			_status = (int8)ROOM_STATUS::FULL;
			_gameLogic.InitGame();
			_timer.Reset();
		}*/
	}

	S2C_ROOM rmpacket;
	rmpacket.size = sizeof(S2C_ROOM);
	rmpacket.type = (int8)S_ROOM_PACKET_TYPE::UPDATE_LIST;
	rmpacket.rmNum = _num;
	rmpacket.member = _mem.load();
	
	{
		READ_SERVER_LOCK;
		ServerIocpCore.BroadCastingAll(&rmpacket);
	}
	
	std::cout << "RM [" << _num << "][" << _cList.size() << "/4]" << std::endl;
	
	// 갱신하는걸 보내줄지 말지 미정
}

void Room::BroadCasting(void* packet) // 방에 속하는 클라이언트에게만 전달하기
{
	// cList Lock 읽기 호출 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto i =  _cList.begin(); i != _cList.end(); ++i)
	{
		if (ServerIocpCore._clients[*i] == nullptr) continue;
		if(!ServerIocpCore._clients[*i]->DoSend(packet))
		{ 
			continue;
		}
	}
}

void Room::BroadCastingExcept(void* packet, int32 sid) // 방에 속하는 클라이언트에게만 전달하기
{
	// cList Lock 읽기 호출 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto i = _cList.begin(); i != _cList.end(); ++i)
	{
		if (ServerIocpCore._clients[*i] == nullptr || *i == sid) continue;
		if (!ServerIocpCore._clients[*i]->DoSend(packet))
		{
			continue;
		}
	}
	
}

// 방에 있는 유저에 대한 게임 로직 업데이트 진행 
void Room::Update()
{
	_timer.Tick(60.f);
	
	
	_gameLogic.Update(_timer.GetTimeElapsed());
	_gameLogic.LateUpdate(_timer.GetTimeElapsed());

	if (_timer.IsTimeToAddHistory())

	{
		_gameLogic.AddHistory();
		S2C_FRAMEPACKET packet;
		packet.size = sizeof(S2C_FRAMEPACKET);
		packet.type = (uint8)S_PACKET_TYPE::FRAME;
		packet.wf = _history.GetCurFrame();
		BroadCasting(&packet);
	}

	if (_timer.IsAfterTick(45)) // 1/45초마다 정확한 위치값을 브로드캐스팅 한다.
	{
		
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (!_gameLogic.IsAvailablePlayer(i)) continue;
			SPlayer& ps = _gameLogic.GetPlayerByIdx(i);

			S2C_POS packet;
			packet.sid = ps.m_sid;
			packet.size = sizeof(S2C_POS);
			packet.type = (uint8)S_PACKET_TYPE::SPOS;
			packet.x = ps.GetPosition().x;
			packet.z = ps.GetPosition().z;
			BroadCasting(&packet);
		}
	}
}

void Room::AddEvent(QueueEvent* qe, float after)
{
	_gameLogic.AddEventAfterTime(after, qe);
}

void Room::AddEvent(QueueEvent* qe)
{
	_gameLogic.AddEvent(qe);
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
	std::cout << "Dead Lock Checking\n";
}
void RoomManager::CreateRoom(int32 sid)
{
	S2C_ROOM_EVENT packet;
	packet.size = sizeof(S2C_ROOM_EVENT);

	if (_rmCnt.load() >= _cap)
	{	
		packet.type = (uint8)S_ROOM_PACKET_TYPE::MK_RM_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		return;
	}
	for (int i = 0; i < 100; ++i)
	{
		if (_rooms[i]._status == (int8)ROOM_STATUS::EMPTY)
		{
			_rooms[i]._status = (int8)ROOM_STATUS::NOT_FULL;
			_rooms[i].UserIn(sid);
			_rmCnt.fetch_add(1);
			break;
		}
	}

	packet.type = (uint8)S_ROOM_PACKET_TYPE::MK_RM_OK;
	ServerIocpCore._clients[sid]->DoSend(&packet);
}
void RoomManager::UpdateRooms()
{
	for (int i = 0; i < _cap; ++i)
	{
		if (_rooms[i]._status != (int8)ROOM_STATUS::FULL) continue;
		_rooms[i].Update();
	}
}
void RoomManager::ExitRoom(int32 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}