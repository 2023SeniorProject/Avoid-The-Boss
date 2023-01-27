#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���

enum ROOM_STATUS 
{
	INGAME = 0 ,FULL = 1,NOT_FULL = 2, COUNT
};

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
public:
	Room(int16 hid): _hid(hid) {}
	~Room() {}
	void ChangeStatus(ROOM_STATUS s) { _status = s; }
	bool DestroyRoom() { return !binary_search(_c_list.begin(), _c_list.end(), _hid); } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
private:
	int16 _rnum = -1; // ���ȣ
	int16 _hid; // ȣ��Ʈ Ŭ���̾�Ʈ ID
	vector<int16> _c_list; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	int8 _status = NOT_FULL; // �� ����
};

class RoomManager
{

public:
	bool CreateRoom(const int16& hid);
	void RemoveRoom(const int16& rm_id);
private:
	map<int16, shared_ptr<Room>> _rooms;
	int16 _rm_cnt = 0; // ���� �� ����;
	int16 _capacity = 1000;

};

