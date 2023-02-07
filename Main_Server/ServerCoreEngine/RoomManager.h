#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���

class GameSession;

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
	template<class T>
	void BroadCasting(T packet);
public:
	list<int16> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	int8 _status = ROOM_STATUS::EMPTY; // �� ����
	uint8 _num = 0;
};

class RoomManager
{

public:
	void ExitRoom(int16 sid, int16 rmNum);
	void EnterRoom(int16 sid, int16 rmNum);
	void CreateRoom(int16 sid);
	void Init();
private:
	Room _rooms[100];
	int16 _rmCnt = 0; // ���� �� ����;
	int16 _cap = 100;
};

