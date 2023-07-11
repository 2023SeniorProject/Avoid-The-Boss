#pragma once
#include "packetEvent.h"


struct JobComparator // �켱 ���� ť���� �񱳿��� ����
{
	bool operator()(const QueueEvent* lhs, const QueueEvent* rhs)
	{
		return lhs->generateTime > rhs->generateTime;
	}
};


typedef std::priority_queue<QueueEvent*, std::vector<QueueEvent*>, JobComparator> JobPriorityQueue;

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
	void PushTask(QueueEvent*, float after); // after �ð� �Ŀ� �ش� �ӹ��� �����Ѵ�.
	void PushTask(QueueEvent*);
	void DoNormalTasks();
	void DoTasks();
	void Clear();
	int64 GetCurrentTick() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _BeginTickPoint).count();
	}

private:

	std::queue<QueueEvent*> _normalQueue;
	JobPriorityQueue _TaskQueue; // Job�� �߻� ƽ�� ���� �켱������ �ο��Ѵ�. --> ƽ���� Ŭ ���� ������ ���̶�� ��
	Clock::time_point _BeginTickPoint; // ���� ���� �ð� ������
	int64 _CurrentTick;  // ���� ƽ
};

