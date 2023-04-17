#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���
#include "CTimer.h"
#include "PlayerInfo.h"


enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};


class Scheduler;
class queueEvent;

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room();
	~Room();
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int32 sid);
	void UserIn(int32 sid);
	void BroadCasting(void* packet);
	void Update();

	PlayerInfo& GetMyPlayerFromRoom(int32 sid) 
	{ 
		std::shared_lock<std::shared_mutex> ll(_listLock);
		auto i = std::find(_cList.begin(), _cList.end(), sid);
		if (i != _cList.end()) return _players[std::distance(_cList.begin(), i)]; // �濡 �ִ� Ŭ���̾�Ʈ���� �˾Ƽ� Ž��
	}
	void AddEvent(queueEvent* packet, float after); // �̺�Ʈ ��Ŷ�� ������ ť���ٰ� �߰��� �� ���̴�.
	void AddEvent(queueEvent* qe);
	void StartGame() 
	{
		_timer.Reset();
	}

public:
	std::shared_mutex _listLock;
	std::list<int32> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	int8 _status = ROOM_STATUS::EMPTY; // �� ����
	int32 _num = 0; // �濡 �ִ� �ο� ��

	Scheduler* _jobQueue; // �濡 ���� �ִ� Ŭ���̾�Ʈ�� �߱��� �̺�Ʈ ť
	std::shared_mutex _jobQueueLock; // eventQueue ������ Lock
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
	Atomic<int32> _rmCnt = 0; // ���� �� ����;
	int32 _cap = 100;
};

