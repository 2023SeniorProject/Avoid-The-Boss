#pragma once
#include "PlayerInfo.h"

struct POS
{
	float x = 0.f;
	float z = 0.f;
public:
	POS& operator=(const XMFLOAT3& playerPos)
	{
		x = playerPos.x;
		z = playerPos.z;
		return *this;
	}
};

class WorldStatus // 서버에서 가지고 있을 정보
{
public:
	WorldStatus() { ResetWorldState(); }
	void ResetWorldState()
	{
		_myWorldFrame = 0;
	}
	void PrintWorldInfo()
	{
		//for (int i = 0; i < PLAYERNUM; ++i) std::cout <<  " )" << "(" << _pPos[i].x << " " << _pPos[i].z << ") ";
		//std::cout << "\n";
	}
public:
	POS _pPos[4]; // 도망자 위치
	XMFLOAT3 _attackLay; // 레이저 방향
	uint32   _myWorldFrame; //자기 자신의 월드 프레임
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
		// 현재 시각보가 과거에 삽입은 안됨
		if (frame < _curFrame)
			return;

		// 현재 시각이 Rewind를 위해 보관할 수 있는 최대를 넘는 경우
		if (frame >= _curFrame + MAX_REWIND)
		{
			_curFrame = frame; // 다시 월드 배열의 첫번째로 되돌아가 채우기 시작한다. 
			_frameIndex = 0;
			_worldHistory.fill(obj);
			return;
		}

		const WorldStatus& prevWorld = _worldHistory[_frameIndex]; // 과거의 월드 상태

		// 현재 프레임 직전까지 바로전 최신값 채우기
		while (_curFrame < frame)
		{
			++_frameIndex;
			if (_frameIndex == MAX_REWIND)
				_frameIndex = 0;

			_worldHistory[_frameIndex] = prevWorld;
			++_curFrame;
		}
		_worldHistory[_frameIndex].PrintWorldInfo();
		_worldHistory[_frameIndex] = obj;
	}

	WorldStatus GetWorldStatusByFrame(uint32 frame) const // 검증을 위해 해당하는 프레임의 월드 상태를 가져온다.
	{
		// 현재 가지고 있는것보다 미래의 값을 요구할 때는 그냥 최신값 리턴
		if (frame > _curFrame)
			return _worldHistory[_frameIndex];

		// 범위밖 과거의 값을 요구할때는 가장 과거값 리턴
		auto delta = _curFrame - frame;
		if (delta >= MAX_REWIND)
		{
			return _worldHistory[(_frameIndex + 1) % MAX_REWIND];
		}
		// 현재 프레임 인덱스 -  프레임 차이값 Ex) 현재 30 프레임까지 값이 채워졌는데 25 프레임의 월드 상태를 구한다 delta = 5 30 + 30 - 5 % 30 = 25;

		return _worldHistory[(_frameIndex + MAX_REWIND - delta) % MAX_REWIND];
	}
	void AddHistory(PlayerInfo* p)
	{
			_lastWorldStatus._pPos[0] = p[0].GetPosition();
			_lastWorldStatus._pPos[1] = p[1].GetPosition();
			_lastWorldStatus._pPos[2] = p[2].GetPosition();
			_lastWorldStatus._pPos[3] = p[3].GetPosition();
			std::cout << _frameIndex << "\n";
			//_lastWorldStatus.PrintWorldInfo();
			++_lastWorldStatus._myWorldFrame;
			SetWorldStatusByFrame(_lastWorldStatus._myWorldFrame, _lastWorldStatus);
	}
	bool IsAttackAvailable(uint32 frame)
	{
		const WorldStatus& cw = GetWorldStatusByFrame(frame);
		// 공격이 검증 가능한 것인지 확인하는 로직

	}

	void Clear()
	{
		_worldHistory.fill(WorldStatus());
		_curFrame = 0;
		_frameIndex = 0;
	}

	uint32 GetCurFrameIdx() const
	{
		return _frameIndex;
	}
private:
	uint32 _curFrame; //   현재 프레임
	uint32 _frameIndex; // 배열내 위치
	WorldStatus _lastWorldStatus;
	std::array<WorldStatus, MAX_REWIND> _worldHistory; // 월드 상태 히스토리
};


