#pragma once

#include "ClientPacketEvent.h"

struct JobComparator // �켱 ���� ť���� �񱳿��� ����
{
	bool operator()(const queueEvent* lhs, const queueEvent* rhs)
	{
		return lhs->generateTime > rhs->generateTime;
	}
};


typedef std::priority_queue<queueEvent*, std::vector<queueEvent*>, JobComparator> JobPriorityQueue;

class Scheduler
{
public:
	using Clock = std::chrono::high_resolution_clock;
	Scheduler();
	void Reset() 
	{
		_BeginTickPoint = Clock::now(); // ���� ������ ����
		_CurrentTick = GetCurrentTick(); // ���� ƽ �� �ʱ�ȭ
	}
	void PushTask(queueEvent*, float after); // after �ð� �Ŀ� �ش� �ӹ��� �����Ѵ�.
	void PushTask(queueEvent*);
	void DoNormalTasks();
	void DoTasks();
	int64 GetCurrentTick() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _BeginTickPoint).count();
	}

private:

	std::queue<queueEvent*> _normalQueue;
	JobPriorityQueue _TaskQueue; // Job�� �߻� ƽ�� ���� �켱������ �ο��Ѵ�. --> ƽ���� Ŭ ���� ������ ���̶�� ��
	Clock::time_point _BeginTickPoint; // ���� ���� �ð� ������
	int64 _CurrentTick;  // ���� ƽ
};

