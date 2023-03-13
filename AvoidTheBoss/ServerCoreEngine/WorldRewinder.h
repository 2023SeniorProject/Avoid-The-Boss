#pragma once
#include "PlayerInfo.h"
#include <chrono>
#include <array>

struct WorldStatus // �������� ������ ���� ����
{
public:
	WorldStatus() { ResetWorldState(); }
	void ResetWorldState()
	{
		_myWorldFrame = 0;
	}
public:
	XMFLOAT3 _pPos[4]; // ������ ��ġ
	XMFLOAT3 _attackLay; // ������ ����
	uint32   _myWorldFrame; //�ڱ� �ڽ��� ���� ������
};


template <uint32 MAX_REWIND>
class Rewinder
{
public:
	Rewinder()
	{
		Clear();
	}

	void SetWorldStatusByFrame(uint32 frame, const WorldStatus& obj)
	{
		// ���� �ð����� ���ſ� ������ �ȵ�
		if (frame < _curFrame)
			return;

		// ���� �ð��� Rewind�� ���� ������ �� �ִ� �ִ븦 �Ѵ� ���
		if (frame >= _curFrame + MAX_REWIND)
		{
			_curFrame = frame; // �ٽ� ���� �迭�� ù��°�� �ǵ��ư� ä��� �����Ѵ�. 
			_frameIndex = 0;
			_worldHistory.fill(obj);
			return;
		}

		const WorldStatus& prevWorld = _worldHistory[_frameIndex]; // ������ ���� ����

		// ���� ������ �������� �ٷ��� �ֽŰ� ä���
		while (_curFrame < frame)
		{
			++_frameIndex;
			if (_frameIndex == MAX_REWIND)
				_frameIndex = 0;

			_worldHistory[_frameIndex] = prevWorld;
			++_curFrame;
		}

		_worldHistory[_frameIndex] = obj;
	}

	WorldStatus GetWorldStatusByFrame(uint32 frame) const // ������ ���� �ش��ϴ� �������� ���� ���¸� �����´�.
	{
		// ���� ������ �ִ°ͺ��� �̷��� ���� �䱸�� ���� �׳� �ֽŰ� ����
		if (frame > _curFrame)
			return _worldHistory[_frameIndex];

		// ������ ������ ���� �䱸�Ҷ��� ���� ���Ű� ����
		auto delta = _curFrame - frame;
		if (delta >= MAX_REWIND)
		{
			return _worldHistory[(_frameIndex + 1) % MAX_REWIND];
		}
		// ���� ������ �ε��� -  ������ ���̰� Ex) ���� 30 �����ӱ��� ���� ä�����µ� 25 �������� ���� ���¸� ���Ѵ� delta = 5 30 + 30 - 5 % 30 = 25;

		return _worldHistory[(_frameIndex + MAX_REWIND - delta) % MAX_REWIND];
	}

	bool IsAttackAvailable(uint32 frame)
	{
		const WorldStatus& cw = GetWorldStatusByFrame(frame);
		// ������ ���� ������ ������ Ȯ���ϴ� ����

	}

	void Clear()
	{
		_worldHistory.fill(WorldStatus());
		_curFrame = 0;
		_frameIndex = 0;
	}

private:
	uint32 _curFrame; //   ���� ������
	uint32 _frameIndex; // �迭�� ��ġ
	std::array<WorldStatus, MAX_REWIND> _worldHistory; // ���� ���� �����丮
};


enum class GameStatus { NONE_GAME, START_GAME, END_GAME};

class GameLogic
{
	using Clock = std::chrono::high_resolution_clock;
public:

	GameLogic() { }
	~GameLogic() { }
	void StartGame() 
	{ 
		gs = GameStatus::START_GAME;
		_accumlateElapsedTime = 0.f;
		_lastTimePoint = Clock::now();
		_initTimePoint = Clock::now();
	}
	void UpdateWorld(float fpsLock, PlayerInfo* p)
	{
		if (gs != GameStatus::START_GAME) return;
		if (fpsLock != 0.0f) _fTimeElapsedAvg = (int)((1.f / fpsLock) * 1000.f);

		std::chrono::time_point curTimePoint = Clock::now();
		std::chrono::duration<float, std::milli> fp_ms = (curTimePoint - _lastTimePoint); // �� �ø����� �ʰ� ��Ȯ�ϰ� ����ϱ�
		float fTimeElapsed = fp_ms.count();
		_lastTimePoint = curTimePoint;
		_accumlateElapsedTime += fTimeElapsed;
		
		for(int i = 0; i < 4; ++i) p[i].Update(fTimeElapsed);
		
		// ���� ƽ �� �̻� �����ϸ�~
		if ( _accumlateElapsedTime >= _fTimeElapsedAvg)
		{
	
			AddHistory(p); // ���� ������ ���¸� ����Ѵ�.
			_accumlateElapsedTime = 0.f;
			
		}

	};
	void AddHistory(PlayerInfo* p)
	{
		 std::cout << " Add WorldStatus Frame : " << _curWorldFrame << "\n";
		_lastWorldStatus._pPos[0] = p[0].GetPosition();
		_lastWorldStatus._pPos[1] = p[1].GetPosition();
		_lastWorldStatus._pPos[2] = p[2].GetPosition();
		_lastWorldStatus._pPos[3] = p[3].GetPosition();
		
		//_lastWorldStatus._attackLay = _chaser.m_xmf3Look; // ������ ����
		_lastWorldStatus._myWorldFrame = _curWorldFrame++;
		_worldHistory.SetWorldStatusByFrame(_lastWorldStatus._myWorldFrame, _lastWorldStatus);
		
	};
public:
	

	uint32 _curWorldFrame = 0;
	WorldStatus _lastWorldStatus; // �׻� �ֽ��� ���� ���¸� �����ϰ� �ִ´�.
	Rewinder<30> _worldHistory; // Ÿ�̸ӷ� ����ؼ�, ���� 1�������� ������ �ȴٸ� �߰��Ѵ�. �ִ� 30�� ���� ���� �����ϴ�.

//
	GameStatus gs = GameStatus::NONE_GAME;
// Ÿ�̸� ���� ��� ����
	Clock::time_point		_lastTimePoint; // ������ ���� �ð�
	Clock::time_point		_StopTimePoint; // ���� ���� 
	Clock::time_point		_initTimePoint;

	float					_fTimeElapsedAvg = 0.f; // �� ������ ó���ϴµ� �ɸ��� ��� �ð�
	float					_accumlateElapsedTime = 0.f; // �� ������ ��ŭ ó�� �ƴ��� Ȯ���ϴ� �뵵
};

//  1. Ŭ��� ������ ������ ��ũ�� �ȸ��� �� ��� �Ұ��� 