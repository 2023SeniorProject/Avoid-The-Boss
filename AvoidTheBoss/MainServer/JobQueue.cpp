#include "pch.h"
#include "JobQueue.h"

Scheduler::Scheduler()
{
	_BeginTickPoint = Clock::now(); // 시작 시점은 지금
	_CurrentTick = GetCurrentTick(); // 현재 틱 값 초기화
}

void Scheduler::PushTask(QueueEvent* task, float after)
{
	_CurrentTick = GetCurrentTick();
	double dueTimeTick = _CurrentTick + after;
	task->generateTime = dueTimeTick;
	_TaskQueue.push(task);
}

void Scheduler::PushTask(QueueEvent* task)
{
	task->generateTime = _CurrentTick;
	_normalQueue.push(task);
}

void Scheduler::DoTasks()
{
	/// tick update
	_CurrentTick = GetCurrentTick(); // 현재 틱값을 구한다.

	while (!_TaskQueue.empty())
	{
		QueueEvent* jobElem = _TaskQueue.top(); // 가장 우선적으로 나와야할 이벤트에 대해서
		if (_CurrentTick < jobElem->generateTime) continue; // 아직 호출할 시점이 되지 않았을 경우 루프를 나온다.
		jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
		_TaskQueue.pop();
		delete jobElem;
		
	}

}

void Scheduler::Clear()
{
	while (_TaskQueue.empty()) _TaskQueue.pop();
	while (_normalQueue.empty()) _TaskQueue.pop();
}


void Scheduler::DoNormalTasks()
{
	/// tick update
	while (!_normalQueue.empty())
	{
		QueueEvent* jobElem = _normalQueue.front(); // 가장 우선적으로 나와야할 이벤트에 대해서
		_normalQueue.pop();
		if (jobElem != nullptr)
		{
			jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
			delete jobElem;
		}
	}
}