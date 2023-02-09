#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���


enum ROOM_STATUS : int8
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room() {}
	~Room() {}
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int16 sid);
	void UserIn(int16 sid);
	void BroadCasting(void* packet);
public:
	RWLOCK;
	vector<int16> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	std::mutex _vecLock;
	int8 _status = ROOM_STATUS::EMPTY; // �� ����
	int32 _num = 0;
};

class RoomManager
{

public:
	void ExitRoom(int16 sid, int16 rmNum);
	void EnterRoom(int16 sid, int16 rmNum);
	void CreateRoom(int16 sid);
	void Init();
public:
	Room _rooms[100];
	Atomic<int32> _rmCnt = 0; // ���� �� ����;
	int32 _cap = 100;
};

