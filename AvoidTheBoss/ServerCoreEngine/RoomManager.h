#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���
#include "CTimer.h"



enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};

class queueEvent;

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room() {}
	~Room() {}
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int32 sid);
	void UserIn(int32 sid);
	void BroadCasting(void* packet);
	void Update();
	void AddEvent(queueEvent* packet); // �̺�Ʈ ��Ŷ�� ������ ť���ٰ� �߰��� �� ���̴�.
public:
	//RWLOCK;
	std::shared_mutex _listLock;
	std::list<int32> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ

	std::queue<queueEvent*> _jobQueue; // �濡 ���� �ִ� Ŭ���̾�Ʈ�� �߱��� �̺�Ʈ ť
	std::shared_mutex _jobQueueLock; // eventQueue ������ Lock
	//
	
	Timer _rmTimer; // �ش� ���� ������Ʈ�� ����� Ÿ�̸�
	int8 _status = ROOM_STATUS::EMPTY; // �� ����
	int32 _num = 0; // �濡 �ִ� �ο� ��
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
	Atomic<int32> _rmCnt = 0; // ���� �� ����;
	int32 _cap = 100;
};
