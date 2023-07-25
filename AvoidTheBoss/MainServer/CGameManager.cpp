#include "pch.h"
#include "CGameManager.h"
#include "jobQueue.h"



CGameManager::CGameManager()
{
	_jobQueue = new Scheduler();
}

CGameManager::~CGameManager()
{
	delete _jobQueue;
}

void CGameManager::InitGame()
{
	// 플레이어 초기 위치 셋팅
	_players[0].SetPosition(XMFLOAT3(0,  0, -18));
	_players[1].SetPosition(XMFLOAT3(10, 0, -18));
	_players[2].SetPosition(XMFLOAT3(15, 0, -18));
	_players[3].SetPosition(XMFLOAT3(20, 0, -18));

	// 발전기 위치 셋팅
	_generators[0]._pos = XMFLOAT3(-23.12724, 1.146619, 1.814123);
	_generators[1]._pos = XMFLOAT3(23.08867, 1.083242, 3.155997);
	_generators[2]._pos = XMFLOAT3(0.6774719, 1.083242, -23.05909);
	
	_generators[0].ResetState();
	_generators[1].ResetState();
	_generators[2].ResetState();

	_gState = GAMESTATE::IN_GAME;
	_history.Clear();
	
}

void CGameManager::Update(float eTime)
{
	if (GAMESTATE::IN_GAME != _gState) return;



	{
		std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue Lock 호출
		_jobQueue->DoTasks();
	}

	for (auto& i : _players) if(!i.m_hide) i.Update(eTime);
}

void CGameManager::LateUpdate(float eTime)
{
	int32 m_activeGenCnt = 0;
	if (GAMESTATE::IN_GAME != _gState) return;
	for (auto& i : _players)if (!i.m_hide) i.LateUpdate(eTime);
	for (auto& i : _generators) if (i._IsActive) m_activeGenCnt++;

	if (m_activeGenCnt >= 3)
	{
		if (!_bExitReady) _bExitReady = true;
	}

	if (_bExitReady) _gState = CheckGameState();
}

void CGameManager::AddEventAfterTime(float after, QueueEvent* qe)
{
	std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue Lock 호출
	_jobQueue->PushTask(qe, after);
}

void CGameManager::AddEvent(QueueEvent* qe)
{
	std::unique_lock<std::shared_mutex> ql(_jobQueueLock); // Queue Lock 호출
	_jobQueue->PushTask(qe);
}

GAMESTATE CGameManager::CheckGameState()
{
	int32 crawlCnt = 0;
	int32 escapeCnt = 0;

	for (auto& i : _players)
	{
		if ((int32)PLAYER_BEHAVIOR::CRAWL == i.GetBehavior()) crawlCnt += 1;
		if (true == i.GetEscaped()) escapeCnt += 1;
	}
	if (PLAYERNUM == crawlCnt) _gState == GAMESTATE::BOSS_WIN;
	else if ((crawlCnt + escapeCnt) == (PLAYERNUM - 1)) GAMESTATE::EMP_WIN;

	return _gState;
}

void CGameManager::ResetGame()
{
	// 월드 히스토리 초기화
	_history.Clear();
	// 플레이어 상태 초기화
	for (auto& i : _players) i.ResetState();
	// 발전기 상태 초기화
	for (auto& i : _generators) i.ResetState();
	// 플레이어 초기 위치 셋팅
	_players[0].SetPosition(XMFLOAT3(0, 0.25, -18));
	_players[1].SetPosition(XMFLOAT3(10, 0.25, -18));
	_players[2].SetPosition(XMFLOAT3(15, 0.25, -18));
	_players[3].SetPosition(XMFLOAT3(20, 0.25, -18));
	_bExitReady = false;

	_gState = GAMESTATE::NONE;

}

