#include "pch.h"
#include "CJobQueue.h"

Scheduler::Scheduler()
{
	_BeginTickPoint = Clock::now(); // 시작 시점은 지금
	_CurrentTick = GetCurrentTick(); // 현재 틱 값 초기화
}

void Scheduler::PushTask(queueEvent* task, float after)
{
	_CurrentTick = GetCurrentTick();
	double dueTimeTick = _CurrentTick + after;
	task->generateTime = dueTimeTick;
	_TaskQueue.push(task);
}

void Scheduler::PushTask(queueEvent* task)
{
	task->generateTime = _CurrentTick;
	_normalQueue.push(task);
}

void Scheduler::DoTasks()
{
	/// tick update

	while (!_TaskQueue.empty())
	{
		_CurrentTick = GetCurrentTick(); // 현재 틱값을 구한다.
		queueEvent* jobElem = _TaskQueue.top(); // 가장 우선적으로 나와야할 이벤트에 대해서
		if (_CurrentTick < jobElem->generateTime)
		{
			std::cout << _CurrentTick << " | " << jobElem->generateTime << " Not Yet\n";
			// 팝하고 다시 집어넣는다.
			_TaskQueue.pop();
			_TaskQueue.push(jobElem);
			continue; // 아직 호출할 시점이 되지 않았을 경우 루프를 나온다.
		}
		jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
		_TaskQueue.pop();
		delete jobElem;
		
	}

}

void Scheduler::DoNormalTasks()
{
	/// tick update
	while (!_normalQueue.empty())
	{
		queueEvent* jobElem = _normalQueue.front(); // 가장 우선적으로 나와야할 이벤트에 대해서
		_normalQueue.pop();
		if (jobElem != nullptr)
		{
			jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
			delete jobElem;
		}
	}

}