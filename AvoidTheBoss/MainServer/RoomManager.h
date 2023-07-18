#pragma once
#define MAX_ROOM_USER 4 // �� ��� �ִ� �ο���
#include "WorldRewinder.h"
#include "SGenerator.h"
#include "SPlayer.h"
#include "CGameManager.h"





class Scheduler;
class QueueEvent;
class SPlayer;

// ���� ȣ��Ʈ�� ��û�ϴ� ���� �����Ѵ�.
class Room
{
	struct Member
	{
		int16 sid = -1;
		bool isReady;
	};
public:
	Room();
	~Room();
	bool IsDestroyRoom() { return (_memCnt.load() == 0);/*(_cList.size() == 0);*/ } // false ��ȯ �� �� �ı� --> ȣ��Ʈ�� ���� ������ ��� �ı��ϵ�����.
	void UserOut(int32 sid);
	void UserIn(int32 sid);
	void BroadCasting(void* packet);
	void BroadCastingExcept(void* packet, int32 sid);
	bool ProcessAttackEvent(const int32& frame, const int16& target) { return (_gameLogic._history.IsAttackAvailable(frame, target)); }
	CGameManager& GetGameManager() { return _gameLogic; }
	void Update();
	void AddEvent(QueueEvent* packet, float after); // �̺�Ʈ ��Ŷ�� ������ ť���ٰ� �߰��� �� ���̴�.
	void AddEvent(QueueEvent* qe);
	void StartGame() 
	{
		_timer.Reset();
	}
	
	void SendRoomListPacket();
	void SendRoomInfoPacket();

	int32 GetSidIndexBySid(int32 sid) 
	{
	
		for (int32 i = 0; i < PLAYERNUM; ++i)
		{
			if (_cArr[i].sid == sid) return i;
		}
		return -1;
	};

	void InitGame();
	void UpdateReady(int32 idx, bool val);
	bool IsGameStartAvailable();
private:
	Rewinder<30> _history;
private:
	CGameManager _gameLogic;
public:
	std::shared_mutex _listLock;
	std::vector<int32> _cList; // �濡 �����ִ� Ŭ���̾�Ʈ ����Ʈ
	Member _cArr[4];
	Atomic<bool> _readys[4];
	int8 _status = (int8)ROOM_STATUS::EMPTY; // �� ����
	int32 _rmNum = 0; // ���ȣ
	Atomic<int32> _memCnt = 0;
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

