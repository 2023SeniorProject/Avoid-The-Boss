#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수
#include "WorldRewinder.h"
#include "SGenerator.h"
#include "SPlayer.h"
#include "CGameManager.h"


enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT = 4
};


class Scheduler;
class QueueEvent;
class SPlayer;

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
	void BroadCastingExcept(void* packet, int32 sid);
	bool ProcessAttackEvent(const int32& frame, const int16& target) { return (_gameLogic._history.IsAttackAvailable(frame, target)); }
	CGameManager& GetGameManager() { return _gameLogic; }
	void Update();
	void AddEvent(QueueEvent* packet, float after); // 이벤트 패킷이 들어오면 큐에다가 추가를 할 것이다.
	void AddEvent(QueueEvent* qe);
	void StartGame() 
	{
		_timer.Reset();
	}
	void SendRoomInfo(); 
private:
	Rewinder<30> _history;
private:
	CGameManager _gameLogic;
public:
	std::shared_mutex _listLock;
	std::list<int32> _cList; // 방에 속해있는 클라이언트 리스트
	int8 _status = ROOM_STATUS::EMPTY; // 방 상태
	int32 _num = 0; // 방에 있는 인원 수
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

