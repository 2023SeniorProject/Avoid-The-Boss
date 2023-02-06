#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���

class GameSession;

enum ROOM_STATUS 
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, EMPTY = 3, COUNT
};

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room() {}
	~Room() {}
	void ChangeStatus(ROOM_STATUS s) { _status = s; }
	bool IsDestroyRoom() { return (_cList.size() == 0); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int16 sid);
	void UserIn(int16 sid);
	void BroadCasting(GameSession& c);
private:
	int16 _hId = -1; // ȣ��Ʈ Ŭ���̾�Ʈ ID
	list<int16> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	int8 _status = EMPTY; // �� ����
	USE_LOCK;
};

class RoomManager
{

public:
	bool CreateRoom(int16 hId);
	void EnterRoom(int16 sid, int16 rmNum);
	void ExitRoom(int16 sid, int16 rmNum);
	void RemoveRoom(int16 rmNum);
private:
	Room _rooms[100];
	int16 _rmCnt = 0; // ���� �� ����;
	int16 _cap = 100;
	
};

