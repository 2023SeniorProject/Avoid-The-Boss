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

class WorldStatus // �������� ������ ���� ����
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
	POS _pPos[4]; // ������ ��ġ
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
		_worldHistory[_frameIndex].PrintWorldInfo();
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
		// ������ ���� ������ ������ Ȯ���ϴ� ����

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
	uint32 _curFrame; //   ���� ������
	uint32 _frameIndex; // �迭�� ��ġ
	WorldStatus _lastWorldStatus;
	std::array<WorldStatus, MAX_REWIND> _worldHistory; // ���� ���� �����丮
};


