#pragma once
#include "packetEvent.h"


struct JobComparator // 우선 순위 큐에서 비교연산 수행
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
	void PushTask(queueEvent*, float after);
	void DoTasks();
	int64 GetCurrentTick() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _BeginTickPoint).count();
	}

private:

	JobPriorityQueue _TaskQueue; // Job의 발생 틱에 따라 우선순위를 부여한다. --> 틱값이 클 수록 오래된 일이라는 것
	Clock::time_point _BeginTickPoint; // 최초 시작 시간 스탬프
	int64 _CurrentTick;  // 현재 틱
};

