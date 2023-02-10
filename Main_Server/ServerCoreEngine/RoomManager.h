#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수


enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};

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
public:
	RWLOCK;
	list<int32> _cList; // 방에 속해있는 클라이언트 리스트
	std::mutex _vecLock;
	int8 _status = ROOM_STATUS::EMPTY; // 방 상태
	int32 _num = 0;
};

class RoomManager
{

public:
	void ExitRoom(int32 sid, int16 rmNum);
	void EnterRoom(int32 sid, int16 rmNum);
	void CreateRoom(int32 sid);
	void Init();
public:
	Room _rooms[100];
	Atomic<int32> _rmCnt = 0; // 현재 방 개수;
	int32 _cap = 100;
};

