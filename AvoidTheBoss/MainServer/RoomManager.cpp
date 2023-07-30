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
	
}

Room::~Room()
{
}

void Room::UserOut(int32 sid)
{
	int idx = 0;

	if (_status == (uint8)ROOM_STATUS::INGAME)
	{
		_gameLogic.GetPlayerBySid(sid).SetVelocity(XMFLOAT3(0, 0, 0)); // �ӵ� 0
		idx = _gameLogic.GetPlayerBySid(sid).m_idx; /// �ε��� ��������
		_gameLogic.GetPlayerBySid(sid).m_hide = true; // ������Ʈ false�� 
		_gameLogic.GetPlayerBySid(sid).SetBehavior(PLAYER_BEHAVIOR::CRAWL);
		
		if (_gameLogic._gState == GAMESTATE::IN_GAME)
		{
			
			if (idx == 0) // ���� �÷��̾ ���� ���
			{
				_gameLogic.ResetGame();
				SC_EVENTPACKET packet;
				packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.eventId = (uint8)EVENT_TYPE::EMP_WIN;
				BroadCastingExcept(&packet, sid);
				

				{
					// cList Lock ���� ȣ��	
					std::unique_lock<std::shared_mutex> wll(_listLock);


					for (int i = 0; i < PLAYERNUM; ++i)
					{
						_cArr[i].sid = -1;
						_cArr[i].isReady = false;
					}
				}
				_memCnt.store(0);
				_status = (uint8)ROOM_STATUS::EMPTY;
				std::cout << "STRANGE GAME END\n";
				return;
			}
			
		}
		else
		{
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.eventId = (uint8)EVENT_TYPE::HIDE_PLAYER_ONE + idx;
			BroadCastingExcept(&packet, sid);
		}
	}
	else
	{
		{
			// cList Lock ���� ȣ��	
			std::unique_lock<std::shared_mutex> wll(_listLock);
			for (int i = 0; i < PLAYERNUM; ++i)
			{
				if (sid == _cArr[i].sid)
				{
					_cArr[i].sid = -1;
					_cArr[i].isReady = false;
					_memCnt.fetch_sub(1);
				}
			}

			if (_status == (uint8)ROOM_STATUS::FULL) _status = (uint8)ROOM_STATUS::NOT_FULL;

			std::cout << "LEFT USER SID LIST [";
			for (auto i : _cArr)  std::cout << (int32)i.sid << "|";
			std::cout << " ]\n";

			SendRoomListPacket();

		}

		// ���� ���ĵǴ� ���.. ������ �����Ѵ�.
		if (IsDestroyRoom())
		{
			_status = (uint8)ROOM_STATUS::EMPTY;
			{
				std::unique_lock<std::shared_mutex> wll(_listLock);
				for (auto& i : _cArr) i.sid = -1;
				for (auto& i : _cArr) i.isReady = false;
			}
			std::cout << "Destroy Room\n";
		}
	}
	
	SendRoomInfoPacket();

	{
		WRITE_SERVER_LOCK
		ServerIocpCore._clients[sid]->_myRm = -1;
	}
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
		std::cout << "Enter Fail\n";
		packet.rmNum = -1;
		packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if (_status == (int8)ROOM_STATUS::NOT_FULL) // �ƴϸ� ���� ����
	{
		packet.rmNum = _rmNum;
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
			std::cout << "LEFT USER SID LIST [";
			for (auto i : _cArr)  std::cout << (int32)i.sid << "|";
			std::cout << " ]\n";

			SendRoomListPacket();

		}
		if (_memCnt == PLAYERNUM)
		{
			_status = (int8)ROOM_STATUS::FULL;
		}
	
	}

	SendRoomInfoPacket();
	// ������ �� ����Ʈ ������ �� ������ ������.
	
	std::cout << "RM [" << _rmNum << "][" << _memCnt.load() << "/4]" << std::endl;
	
	// �����ϴ°� �������� ���� ����
}

void Room::BroadCasting(void* packet) // �濡 ���ϴ� Ŭ���̾�Ʈ���Ը� �����ϱ�
{
	// cList Lock �б� ȣ�� 
	std::shared_lock<std::shared_mutex> rll(_listLock);
	
	for (auto i : _cArr)
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
	
	for (auto i : _cArr)
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


	// ������ �����ٸ� ������ �����ٴ� ��Ŷ ����
	if (GAMESTATE::EMP_WIN == _gameLogic._gState || GAMESTATE::BOSS_WIN == _gameLogic._gState)
	{
		_status = (uint8)ROOM_STATUS::FULL;

		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		if (GAMESTATE::BOSS_WIN == _gameLogic._gState) packet.eventId = (uint8)EVENT_TYPE::BOSS_WIN;
		else  packet.eventId = (uint8)EVENT_TYPE::EMP_WIN;

		BroadCasting(&packet);
		std::cout << "Normal Game END\n";

		for (auto& i : _cArr)
		{
			if (i.sid != -1) UserOut(i.sid);
		}

		_gameLogic.ResetGame();
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
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			rmifpacket.sids[i] = _cArr[i].sid;
			rmifpacket.rd[i] = _cArr[i].isReady;
		}

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

		{
			for (int i = 0; i < PLAYERNUM; ++i)
			{
				packet.sids[i] = _cArr[i].sid;
				_gameLogic.SetPlayerSidAndIdx(_cArr[i].sid, i);
			}
		}
		BroadCasting(&packet);
		std::cout << "TOTAL USER SID LIST[";
		
		for (int i = 0; i < 4; ++i) std::cout << _gameLogic._players[i].m_sid << " | ";
		std::cout << "]\n";

		{
			shared_lock<std::shared_mutex> rl(_listLock);
			for (auto& i : _cArr) i.isReady = false;
		}
		_gameLogic.InitGame();
		_timer.Reset();
		_status = (uint8)ROOM_STATUS::INGAME;
	}
}
void Room::UpdateReady(int32 idx, bool val)
{
	_cArr[idx].isReady = val;	
}

bool Room::IsGameStartAvailable()
{
	 int cnt = 0;  

	 {
		 shared_lock<std::shared_mutex> rl(_listLock);
		 for (int i = 0; i < 4; ++i) if (_cArr[i].isReady) ++cnt;
		 return (PLAYERNUM == cnt);
	 }
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