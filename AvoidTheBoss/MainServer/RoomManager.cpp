#include "pch.h"
#include "RoomManager.h"
#include "CSIocpCore.h"
#include "JobQueue.h"


using namespace std;
//========== ROOM =============

//    cList Lock 호출 타이밍
//    1. 쓰는 경우 : 유저가 방에 들어오거나 나갈 때, cList 값을 갱신할 때
//    2. 읽는 경우 : 방에 있는 유저에게 브로드 캐스팅 진행하는 경우 cList를 탐색할 때

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

	if (_gameLogic._gState == GAMESTATE::IN_GAME)
	{
		_gameLogic.GetPlayerBySid(sid).SetVelocity(XMFLOAT3(0, 0, 0)); // 속도 0
		idx = _gameLogic.GetPlayerBySid(sid).m_idx; /// 인덱스 가져오기
		_gameLogic.GetPlayerBySid(sid).m_hide = true; // 업데이트 false로 
		
		if (idx == 0) // 사장 플레이어가 나간 경우
		{
			_gameLogic.ResetGame();
			SC_EVENTPACKET packet;
			packet.type = (uint8)EVENT_TYPE::GAME_END;
			packet.size = sizeof(SC_EVENTPACKET);
			BroadCastingExcept(&packet, sid);
		}
		else 
		{
			_gameLogic.GetPlayerBySid(sid).SetBehavior(PLAYER_BEHAVIOR::CRAWL);
		}
	}

	{
		// cList Lock 쓰기 호출	
		std::unique_lock<std::shared_mutex> wll(_listLock);
		_memCnt.fetch_sub(1);
		
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (sid == _cArr[i].sid)
			{
				_cArr[i].sid = -1;
				_cArr[i].isReady = false;
			}
		}
		SendRoomListPacket();
	}

	

	std::cout << "LEFT USER SID LIST [";
	for (auto i : _cArr) if(-1 != i.sid) std::cout << (int32)i.sid << "|";
	std::cout << " ]\n";
	
	if (_status == (uint8)ROOM_STATUS::INGAME)
	{
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.eventId = (uint8)EVENT_TYPE::HIDE_PLAYER_ONE + idx;
		BroadCasting(&packet);
	}
	else if (_status == (uint8)ROOM_STATUS::NOT_FULL /*&& _gameLogic._gState == GAMESTATE::NONE*/) // 아직 게임중이 아닐 경우~
	{
		S2C_ROOM_READY rcpacket;
		rcpacket.type = (uint8)S_ROOM_PACKET_TYPE::REP_READY_CANCEL;
		rcpacket.size = sizeof(S2C_ROOM_READY);
		rcpacket.sid = sid;

		BroadCasting(&rcpacket);
		SendRoomInfoPacket();
	}
	
	// 방이 폭파되는 경우.. 게임을 리셋한다.
	if (IsDestroyRoom()) 
	{
		_gameLogic.ResetGame();
		_status = (uint8)ROOM_STATUS::EMPTY;
		for (auto& i : _cArr) i.isReady = false;
		std::cout << "Destroy Room\n";
	}
	
}

void Room::UserIn(int32 sid)
{

	std::cout << sid << " Try Enter Room\n";
	S2C_ROOM_ENTER packet;
	packet.size = sizeof(S2C_ROOM_ENTER);
	packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_OK;
	

	// 비어있는 방 (만들어지지 않은 방) or 현재 인원수가 4명이면 접속 실패 
	if (_status == (int8)ROOM_STATUS::FULL || _status == (int8)ROOM_STATUS::EMPTY || _status == (int8)ROOM_STATUS::INGAME)
	{
		// enter fail
		std::cout << "Enter Fail\n";
		packet.rmNum = -1;
		packet.type = (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_FAIL;
		ServerIocpCore._clients[sid]->DoSend(&packet);
	}
	else if (_status == (int8)ROOM_STATUS::NOT_FULL) // 아니면 접속 성공
	{
		packet.rmNum = _rmNum;
		ServerIocpCore._clients[sid]->_myRm = _rmNum;
		ServerIocpCore._clients[sid]->DoSend(&packet);
		{
			//cList Lock 쓰기 호출 
			std::unique_lock<std::shared_mutex> wll(_listLock);
			_memCnt.fetch_add(1);
			// 빈 배열 자리에다가 정보 채우기
			for (int k = 0; k < PLAYERNUM; ++k)
			{
				if (-1 == _cArr[k].sid)
				{
					_cArr[k].sid = sid;
					break;
				}
			}
			SendRoomListPacket();

		}
		if (_memCnt == PLAYERNUM)
		{
			_status = (int8)ROOM_STATUS::FULL;
		}
	
	}

	SendRoomInfoPacket();
	// 갱신할 방 리스트 정보와 방 정보를 보낸다.
	
	std::cout << "RM [" << _rmNum << "][" << _memCnt.load() << "/4]" << std::endl;
	
	// 갱신하는걸 보내줄지 말지 미정
}

void Room::BroadCasting(void* packet) // 방에 속하는 클라이언트에게만 전달하기
{
	// cList Lock 읽기 호출 
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

void Room::BroadCastingExcept(void* packet, int32 sid) // 방에 속하는 클라이언트에게만 전달하기
{
	// cList Lock 읽기 호출 
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

// 방에 있는 유저에 대한 게임 로직 업데이트 진행 
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

	if (_timer.IsAfterTick(45)) // 1/45초마다 정확한 위치값을 브로드캐스팅 한다.
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


	// 게임이 끝났다면 게임이 끝났다는 패킷 전송
	if (GAMESTATE::EMP_WIN == _gameLogic._gState || GAMESTATE::BOSS_WIN == _gameLogic._gState)
	{
		_gameLogic.ResetGame();

		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.eventId = (uint8)EVENT_TYPE::GAME_END;
		BroadCasting(&packet);
		std::cout << "Game END\n";
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
	S2C_ROOM_LIST rmpacket; // 로비에서 리스트를 갱신하기 위한 패킷
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
	S2C_ROOM_INFO rmifpacket; // 게임 방에 현재 들어와 있는 멤버들의 리스트를 보내주기 위한 패킷, 방 유저에게만 전송
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
			_gameLogic.SetPlayerSidAndIdx(_cArr[i].sid,i);
		}
		BroadCasting(&packet);
		std::cout << "TOTAL USER SID LIST[";
		
		for (int i = 0; i < 4; ++i) std::cout << _gameLogic._players[i].m_sid << " | ";
		std::cout << "]\n";

		for (auto& i : _cArr) i.isReady = false;
		
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
	 for (int i = 0; i < 4; ++i) if (_cArr[i].isReady) ++cnt; 
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