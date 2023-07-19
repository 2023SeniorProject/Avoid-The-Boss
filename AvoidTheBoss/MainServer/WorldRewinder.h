#pragma once
#include "SPlayer.h"

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
	XMFLOAT3 GetPos(int32 idx) { return XMFLOAT3(_pPos[idx].x, 0.f, _pPos[idx].z); }
	void PrintWorldInfo() {}
public:
	POS			_pPos[4];		//  �÷��̾� ��ġ
	uint32		_myWorldFrame; //  �ڱ� �ڽ��� ���� ������
	XMFLOAT3	_bossDir;
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
		if (frame > _curFrame) return _worldHistory[_frameIndex];

		// ������ ������ ���� �䱸�Ҷ��� ���� ���Ű� ����
		auto delta = _curFrame - frame;
		if (delta >= MAX_REWIND)
		{
			return _worldHistory[(_frameIndex + 1) % MAX_REWIND];
		}
		// ���� ������ �ε��� -  ������ ���̰� Ex) ���� 30 �����ӱ��� ���� ä�����µ� 25 �������� ���� ���¸� ���Ѵ� delta = 5 30 + 30 - 5 % 30 = 25;

		return _worldHistory[(_frameIndex + MAX_REWIND - delta) % MAX_REWIND];
	}
	void AddHistory(SPlayer* players)
	{	
		    _lastWorldStatus._pPos[0] = players[0].GetPosition();
			_lastWorldStatus._bossDir = players[0].GetLook();

			_lastWorldStatus._pPos[1] = players[1].GetPosition();
			_lastWorldStatus._pPos[2] = players[2].GetPosition();
			_lastWorldStatus._pPos[3] = players[3].GetPosition();
			
			++_lastWorldStatus._myWorldFrame;
			SetWorldStatusByFrame(_lastWorldStatus._myWorldFrame, _lastWorldStatus);
	}
	bool IsAttackAvailable(uint32 frame, int32 targetIdx)
	{
		WorldStatus cw = GetWorldStatusByFrame(frame);
		// ������ ���� ������ ������ Ȯ���ϴ� ����
		float rayDist = 10.0f;
		XMFLOAT3 bossLoc = cw.GetPos(0);
		XMFLOAT3 targetLoc = cw.GetPos(targetIdx);
		XMFLOAT3 layDir = cw._bossDir;
		BoundingSphere playerBV;
		playerBV.Center = targetLoc;
		playerBV.Radius = 0.3f;

		// RTT�� ����ؼ� 5 �������̻� ���̱����� ���� ��Ŷ�� �ƴ� ������ �����Ѵ�.
		if (frame + 5 <= _curFrame) std::cout << "Past Attacked Packet Detected. CurFrame :" << _curFrame << " Packet Frame : " << frame << "\n";
		if (playerBV.Intersects(XMLoadFloat3(&bossLoc), XMLoadFloat3(&layDir), rayDist))
		{
			return true;
		}
		return false;
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

	uint32 GetCurFrame() const
	{
		return _curFrame;
	}
private:
	uint32 _curFrame; //   ���� ������
	uint32 _frameIndex; // �迭�� ��ġ
	WorldStatus _lastWorldStatus; // ���� �ֽ� ���� ����
	std::array<WorldStatus, MAX_REWIND> _worldHistory; // ���� ���� �����丮
};


