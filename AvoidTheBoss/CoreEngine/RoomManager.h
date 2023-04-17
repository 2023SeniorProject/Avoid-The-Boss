#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수
#include "CTimer.h"
#include "PlayerInfo.h"


enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};


class Scheduler;
class queueEvent;

// 방은 호스트가 요청하는 순간 생성한다.
class Room
{
public:
	Room();
	~Room();
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false 반환 시 방 파괴 --> 호스트가 방을 나갔을 경우 파괴하도록함.
	void UserOut(int32 sid);
	void UserIn(int32 sid);
	void BroadCasting(void* packet);
	void Update();

	PlayerInfo& GetMyPlayerFromRoom(int32 sid) 
	{ 
		std::shared_lock<std::shared_mutex> ll(_listLock);
		auto i = std::find(_cList.begin(), _cList.end(), sid);
		if (i != _cList.end()) return _players[std::distance(_cList.begin(), i)]; // 방에 있는 클라이언트들을 알아서 탐색
	}
	void AddEvent(queueEvent* packet, float after); // 이벤트 패킷이 들어오면 큐에다가 추가를 할 것이다.
	void AddEvent(queueEvent* qe);
	void StartGame() 
	{
		_timer.Reset();
	}

public:
	std::shared_mutex _listLock;
	std::list<int32> _cList; // 방에 속해있는 클라이언트 리스트
	int8 _status = ROOM_STATUS::EMPTY; // 방 상태
	int32 _num = 0; // 방에 있는 인원 수

	Scheduler* _jobQueue; // 방에 속해 있는 클라이언트가 야기한 이벤트 큐
	std::shared_mutex _jobQueueLock; // eventQueue 관리용 Lock
	PlayerInfo _players[4];
	Timer _timer;


};

class RoomManager
{

public:
	void ExitRoom(int32 sid, int16 rmNum);
	void EnterRoom(int32 sid, int16 rmNum);
	void CreateRoom(int32 sid);
	void UpdateRooms();
	Room& GetRoom(int32 rmNum) { return _rooms[rmNum]; }
	void Init();
public:
	Room _rooms[100];
	Atomic<int32> _rmCnt = 0; // 현재 방 개수;
	int32 _cap = 100;
};

