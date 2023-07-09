#pragma once
#include "SPlayer.h"
#include "SGenerator.h"
#include "WorldRewinder.h"

class Scheduler;
class QueueEvent;

enum class GAMESTATE { NONE, IN_GAME, BOSS_WIN, EMP_WIN };


class CGameManager
{
	friend class Room;
protected:
	SPlayer _players[4];
	SGenerator _generators[3];
	GAMESTATE _gState = GAMESTATE::NONE;
	std::shared_mutex _jobQueueLock; // eventQueue 관리용 Lock
protected:
	Rewinder<30> _history;
	Scheduler* _jobQueue; // 방에 속해 있는 클라이언트가 야기한 이벤트 큐
public:
	CGameManager();
	~CGameManager();

	void InitGame();
	void Update(float);
	void LateUpdate(float);

	bool IsAvailablePlayer(int32 idx) { return (!_players[idx].m_hide && !Vector3::IsZero(_players[idx].GetVelocity())); }
	SPlayer& GetPlayerBySid(int32 sid)
	{
		for (auto& i : _players) if (sid == i.m_sid) return _players[i.m_idx];
	}
	SPlayer& GetPlayerByIdx(int32 idx) { return _players[idx]; }
	SGenerator& GetGeneratorByIdx(int32 idx) { return _generators[idx]; }


	void AddEventAfterTime(float time, QueueEvent*);
	void AddEvent(QueueEvent*);
	void AddHistory()
	{
		_history.AddHistory(_players);
	}
	void SetPlayerSidAndIdx(int32 sid, int32 idx) { _players[idx].m_idx = idx; _players[idx].m_sid = sid; }

	GAMESTATE CheckGameState();
	void ResetGame();
	

};

