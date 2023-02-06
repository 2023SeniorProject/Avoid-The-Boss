#pragma once
#define MAX_ROOM_USER 4 // 한 방당 최대 인원수

enum ROOM_STATUS 
{
	INGAME = 0 ,FULL = 1, NOT_FULL = 2, COUNT
};

// 방은 호스트가 요청하는 순간 생성한다.
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
	bool IsDestroyRoom() { return !binary_search(_cList.begin(), _cList.end(), _hId); } // false 반환 시 방 파괴 --> 호스트가 방을 나갔을 경우 파괴하도록함.
	void UserOut(int16 sid);
	void UserIn(int16 sid);
private:
	int16 _hId = -1; // 호스트 클라이언트 ID
	list<int16> _cList; // 방에 속해있는 클라이언트 리스트
	int8 _status = NOT_FULL; // 방 상태
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
	int16 _rmCnt = 0; // 현재 방 개수;
	int16 _cap = 100;

};

