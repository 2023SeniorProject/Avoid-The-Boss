#include "pch.h"
#include "JobQueue.h"

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

	_CurrentTick = GetCurrentTick();
	double dueTimeTick = _CurrentTick;
	task->generateTime = dueTimeTick;
	_normalQueue.push(task);
}

void Scheduler::DoTasks()
{
	/// tick update
	_CurrentTick = GetCurrentTick(); // 현재 틱값을 구한다.

	while (!_TaskQueue.empty())
	{
		queueEvent* jobElem = _TaskQueue.top(); // 가장 우선적으로 나와야할 이벤트에 대해서
		if (_CurrentTick < jobElem->generateTime) continue; // 아직 호출할 시점이 되지 않았을 경우 루프를 나온다.
		jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
		_TaskQueue.pop();
		std::cout << "do job\n" << std::endl;
		delete jobElem;
		
	}

}


void Scheduler::DoNormalTasks()
{
	/// tick update
	
	while (!_normalQueue.empty())
	{
		queueEvent* jobElem = _normalQueue.front(); // 가장 우선적으로 나와야할 이벤트에 대해서
		jobElem->Task(); // 만약 호출할 시점이 됐다면 해당 잡을 수행하고 queue에서 제거
		_normalQueue.pop();
		std::cout << "do job\n" << std::endl;
		delete jobElem;

	}

}