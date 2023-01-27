#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수

enum ROOM_STATUS 
{
	INGAME = 0 ,FULL = 1,NOT_FULL = 2, COUNT
};

// 방은 호스트가 요청하는 순간 생성한다.
class Room
{
public:
	Room(int16 hid): _hid(hid) {}
	~Room() {}
	void ChangeStatus(ROOM_STATUS s) { _status = s; }
	bool DestroyRoom() { return !binary_search(_c_list.begin(), _c_list.end(), _hid); } // false 반환 시 방 파괴 --> 호스트가 방을 나갔을 경우 파괴하도록함.
private:
	int16 _rnum = -1; // 방번호
	int16 _hid; // 호스트 클라이언트 ID
	vector<int16> _c_list; // 방에 속해있는 클라이언트 리스트
	int8 _status = NOT_FULL; // 방 상태
};

class RoomManager
{

public:
	bool CreateRoom(const int16& hid);
	void RemoveRoom(const int16& rm_id);
private:
	map<int16, shared_ptr<Room>> _rooms;
	int16 _rm_cnt = 0; // 현재 방 개수;
	int16 _capacity = 1000;

};

