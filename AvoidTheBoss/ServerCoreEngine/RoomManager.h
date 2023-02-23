#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수
#include "STimer.h"



enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};

class queueEvent;

// 방은 호스트가 요청하는 순간 생성한다.
class Room
{
public:
	Room() {}
	~Room() {}
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false 반환 시 방 파괴 --> 호스트가 방을 나갔을 경우 파괴하도록함.
	void UserOut(int32 sid);
	void UserIn(int32 sid);
	void BroadCasting(void* packet);
	void Update();
	void AddEvent(queueEvent* packet); // 이벤트 패킷이 들어오면 큐에다가 추가를 할 것이다.
public:
	//RWLOCK;
	std::shared_mutex _listLock;
	std::list<int32> _cList; // 방에 속해있는 클라이언트 리스트
	//std::queue<queueEvent*> _jobQueue; // 방에 속해 있는 클라이언트가 야기한 이벤트 큐
	//std::shared_mutex _jobQueueLock; // eventQueue 관리용 Lock
	//
	
	STimer _rmTimer; // 해당 룸의 업데이트에 사용할 타이머
	int8 _status = ROOM_STATUS::EMPTY; // 방 상태
	int32 _num = 0; // 방에 있는 인원 수
};

class RoomManager
{

public:
	void ExitRoom(int32 sid, int16 rmNum);
	void EnterRoom(int32 sid, int16 rmNum);
	void CreateRoom(int32 sid);
	void UpdateRooms();
	void Init();
public:
	Room _rooms[100];
	Atomic<int32> _rmCnt = 0; // 현재 방 개수;
	int32 _cap = 100;
};

