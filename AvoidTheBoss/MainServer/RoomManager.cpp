#include "pch.h"
#include "RoomManager.h"
#include "CSIocpCore.h"
#include "JobQueue.h"


using namespace std;
//========== ROOM =============

//    cList Lock ȣ�� Ÿ�̹�
//    1. ���� ��� : ������ �濡 �����ų� ���� ��, cList ���� ������ ��
//    2. �д� ��� : �濡 �ִ� �������� ��ε� ĳ���� �����ϴ� ��� cList�� Ž���� ��

//=============================
Room::Room()
{
	//_cList.reserve(PLAYERNUM);
}

Room::~Room()
{
}

void Room::UserOut(int32 sid)
{
	int idx = 0;
	_gameLogic.GetPlayerBySid(sid).SetVelocity(XMFLOAT3(0, 0, 0)); // �ӵ� 0
	idx = _gameLogic.GetPlayerBySid(sid).m_idx; /// �ε��� ��������
	_gameLogic.GetPlayerBySid(sid).m_hide = true; // ������Ʈ false�� ����

	{
		// cList Lock ���� ȣ��	
		//std::unique_lock<std::shared_mutex> wll(_listLock);
		//auto i = std::find(_cList.begin(), _cList.end(), sid); // ����Ʈ�� �ִ��� Ž�� ��
		//if (i != _cList.end()) _cList.erase(i); // ����Ʈ���� ����
		_memCnt.fetch_sub(1);
		
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (sid == _cArr[i].sid)
			{
				_cArr[i].sid = -1;
				_cArr[i].isReady = false;
			}
		}


		if((uint8)ROOM_STATUS::NOT_FULL == _status)
		{
			SendRoomInfoPacket();
		}

		SendRoomListPacket();
	}
		
	std::cout << "LEFT USER SID LIST [";
	//for (auto i : _cList) std::cout << i << ", ";
	for (auto i : _cArr) if(-1 != i.sid) std::cout << i.sid << ", ";
	std::cout << " ]\n";
	
	// �� �����߿�  ���� �÷��̾�� ���⵵�� �Ѵ�.
	if (_status == (uint8)ROOM_STATUS::INGAME) 
	{
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.eventId = (uint8)EVENT_TYPE::HIDE_PLAYER_ONE + idx;
		BroadCasting(&packet);
	}
	else if (_status == (uint8)ROOM_STATUS::NOT_FULL)
	{
		S2C_ROOM_READY packet;
		packet.size = sizeof(S2C_ROOM_READY);
		packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_READY_CANCEL;
		packet.sid = sid;
		BroadCastingExcept(&packet, sid);
	}

	if (IsDestroyRoom())
	{
		_status = (uint8)ROOM_STATUS::EMPTY;
		
		std::cout << "Destroy Room\n";
	}
	std::cout << "RM [" << _rmNum << "][" << _memCnt.load() << "/4]" << std::endl;
}

void Room::UserIn(int32 sid)
{

	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_OK;

	// ����ִ� �� (��������� ���� ��) or ���� �ο����� 4���̸� ���� ���� 
	if (_status == (int8)ROOM_STATUS::FULL || _status == (int8)ROOM_STATUS::EMPTY || _status == (int8)ROOM_STATUS::INGAME)
	{
		// enter fail
		packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if (_status == (int8)ROOM_STATUS::NOT_FULL) // �ƴϸ� ���� ����
	{

		ServerIocpCore._clients[sid]->_myRm = _rmNum;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		{
			//cList Lock ���� ȣ�� 
			std::unique_lock<std::shared_mutex> wll(_listLock);
			_memCnt.fetch_add(1);
			// �� �迭 �ڸ����ٰ� ���� ä���
			for (int k = 0; k < PLAYERNUM; ++k)
			{
				if (-1 == _cArr[k].sid)
				{
					_cArr[k].sid = sid;
					break;
				}
			}
		
		}
		if (_memCnt == PLAYERNUM)
		{
			_status = (int8)ROOM_STATUS::FULL;
		}
	
	}

	SendRoomInfoPacket();
	// ������ �� ����Ʈ ������ �� ������ ������.
	SendRoomListPacket();
	
	std::cout << "RM [" << _rmNum << "][" << _memCnt.load() << "/4]" << std::endl;
	
	// �����ϴ°� �������� ���� ����
}

void Room::BroadCasting(void* packet) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
{
	// cList Lock �б� ȣ�� 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto& i : _cArr)
	{
		if (-1 == i.sid) continue;
		else 
		{
			
			if (ServerIocpCore._clients[i.sid] == nullptr) continue;
			if (!ServerIocpCore._clients[i.sid]->DoSend(packet))
			{
				continue;
			}
		}
	}
}

void Room::BroadCastingExcept(void* packet, int32 sid) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
{
	// cList Lock �б� ȣ�� 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	for (auto& i : _cArr)
	{
		if (-1 == i.sid || sid == i.sid) continue;
		else
		{
			if (ServerIocpCore._clients[i.sid] == nullptr) continue;
			if (!ServerIocpCore._clients[i.sid]->DoSend(packet))
			{
				continue;
			}
		}
	}
}

// �濡 �ִ� ������ ���� ���� ���� ������Ʈ ���� 
void Room::Update()
{
	if (_status != (int8)ROOM_STATUS::INGAME) return;
	_timer.Tick(60.f);
	_gameLogic.Update(_timer.GetTimeElapsed());
	_gameLogic.LateUpdate(_timer.GetTimeElapsed());

	if (_timer.IsTimeToAddHistory())

	{
		
		_gameLogic.AddHistory();
		S2C_FRAMEPACKET packet;
		packet.size = sizeof(S2C_FRAMEPACKET);
		packet.type = (uint8)S_GAME_PACKET_TYPE::FRAME;
		packet.wf = _gameLogic._history.GetCurFrame();
		BroadCasting(&packet);
	}

	if (_timer.IsAfterTick(45)) // 1/45�ʸ��� ��Ȯ�� ��ġ���� ��ε�ĳ���� �Ѵ�.
	{
	
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			//if (!_gameLogic.IsAvailablePlayer(i)) continue;
			SPlayer& ps = _gameLogic.GetPlayerByIdx(i);

			S2C_POS packet;
			packet.sid = ps.m_sid;
			packet.size = sizeof(S2C_POS);
			packet.type = (uint8)S_GAME_PACKET_TYPE::SPOS;
			packet.x = ps.GetPosition().x;
			packet.z = ps.GetPosition().z;
			BroadCasting(&packet);
			
		}
	}
}

void Room::AddEvent(QueueEvent* qe, float after)
{
	_gameLogic.AddEventAfterTime(after, qe);
}

void Room::AddEvent(QueueEvent* qe)
{
	_gameLogic.AddEvent(qe);
}
void Room::SendRoomListPacket()
{
	S2C_ROOM_LIST rmpacket; // �κ񿡼� ����Ʈ�� �����ϱ� ���� ��Ŷ
	rmpacket.size = sizeof(S2C_ROOM_LIST);
	rmpacket.type = (int8)S_ROOM_PACKET_TYPE::UPDATE_LIST;
	rmpacket.member = _memCnt.load();
	rmpacket.rmNum = _rmNum;

	{
		READ_SERVER_LOCK;
		ServerIocpCore.BroadCastingAll(&rmpacket);
	}
	
}
void Room::SendRoomInfoPacket()
{
	S2C_ROOM_INFO rmifpacket; // ���� �濡 ���� ���� �ִ� ������� ����Ʈ�� �����ֱ� ���� ��Ŷ, �� �������Ը� ����
	rmifpacket.size = sizeof(S2C_ROOM_INFO);
	rmifpacket.type = (uint8)S_ROOM_PACKET_TYPE::ROOM_INFO;
	{
		shared_lock<std::shared_mutex> rl(_listLock);
		for (int i = 0; i < PLAYERNUM; ++i) rmifpacket.sids[i] = _cArr[i].sid;
	}
	BroadCasting(&rmifpacket);
	
}

void Room::InitGame()
{
	
	if (IsGameStartAvailable())
	{
		S2C_GAMESTART packet;
		packet.size = sizeof(S2C_GAMESTART);
		packet.type = (uint8)S_ROOM_PACKET_TYPE::GAME_START;
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			packet.sids[i] = _cArr[i].sid;
			_gameLogic._players[i].m_sid = _cArr[i].sid;
		}
		BroadCasting(&packet);

		std::cout << "GAME START\n";
		std::cout << "TOTAL USER SID LIST[";
		
		for(int i = 0; i < 4; ++i) 
		std::cout << "]\n";
		
		
		
		_gameLogic.InitGame();
		_timer.Reset();
		_status = (uint8)ROOM_STATUS::INGAME;
	}
}
void Room::UpdateReady(int32 idx, bool val)
{
	_readys[idx].store(val);
	
}

bool Room::IsGameStartAvailable()
{
	 int cnt = 0;  
	 for (int i = 0; i < 4; ++i) if (_readys[i]) ++cnt; 
	 return (PLAYERNUM == cnt); 
}

// ======= RoomManager ========

// ============================
void RoomManager::Init()
{
	for (int i = 0; i < _cap; ++i)
	{
		_rooms[i]._cList.clear();
		_rooms[i]._rmNum = i;
	}
}
void RoomManager::EnterRoom(int32 sid,int16 rmNum)
{
	_rooms[rmNum].UserIn(sid);
}
void RoomManager::CreateRoom(int32 sid)
{
	S2C_ROOM_EVENT packet;
	packet.size = sizeof(S2C_ROOM_EVENT);

	if (_rmCnt.load() >= _cap)
	{	
		packet.type = (uint8)S_ROOM_PACKET_TYPE::MK_RM_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		return;
	}
	for (int i = 0; i < 100; ++i)
	{
		if (_rooms[i]._status == (int8)ROOM_STATUS::EMPTY)
		{
			_rooms[i]._status = (int8)ROOM_STATUS::NOT_FULL;
			EnterRoom(sid, i);
			_rmCnt.fetch_add(1);
			break;
		}
	}

	packet.type = (uint8)S_ROOM_PACKET_TYPE::MK_RM_OK;
	ServerIocpCore._clients[sid]->DoSend(&packet);
}
void RoomManager::UpdateRooms()
{
	for (int i = 0; i < _cap; ++i)
	{
		if (_rooms[i]._status != (int8)ROOM_STATUS::INGAME) continue;
		_rooms[i].Update();
	}
}
void RoomManager::ExitRoom(int32 sid, int16 rmNum)
{
	_rooms[rmNum].UserOut(sid);
}