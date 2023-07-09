#pragma once
#include "packetEvent.h"


struct JobComparator // 우선 순위 큐에서 비교연산 수행
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
		_BeginTickPoint = Clock::now(); // 시작 시점은 지금
		_CurrentTick = GetCurrentTick(); // 현재 틱 값 초기화
	}
	void PushTask(QueueEvent*, float after); // after 시간 후에 해당 임무를 수행한다.
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
	JobPriorityQueue _TaskQueue; // Job의 발생 틱에 따라 우선순위를 부여한다. --> 틱값이 클 수록 오래된 일이라는 것
	Clock::time_point _BeginTickPoint; // 최초 시작 시간 스탬프
	int64 _CurrentTick;  // 현재 틱
};

