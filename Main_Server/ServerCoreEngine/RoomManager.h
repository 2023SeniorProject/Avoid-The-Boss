#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���

enum ROOM_STATUS 
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, COUNT
};

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room() {}
	Room(int16 hid): _hId(hid) 
	{
		_cList.push_back(_hId);
	}
	~Room() {}
	void ChangeStatus(ROOM_STATUS s) { _status = s; }
	bool IsDestroyRoom() { return !binary_search(_cList.begin(), _cList.end(), _hId); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int16 sid);
	void UserIn(int16 sid);
private:
	int16 _hId = -1; // ȣ��Ʈ Ŭ���̾�Ʈ ID
	list<int16> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	int8 _status = NOT_FULL; // �� ����
};

class RoomManager
{

public:
	bool CreateRoom(int16 hId);
	void EnterRoom(int16 sid, int16 rnNum);
	void RemoveRoom(int16 rmNum);
	void BroadCasting();
private:
	std::map<int32 , Room> _rooms;
	int16 _rmCnt = 0; // ���� �� ����;
	int16 _cap = 100;

};

