#include "pch.h"
#include "JobQueue.h"

Scheduler::Scheduler()
{
	_BeginTickPoint = Clock::now(); // ���� ������ ����
	_CurrentTick = GetCurrentTick(); // ���� ƽ �� �ʱ�ȭ
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
	_CurrentTick = GetCurrentTick(); // ���� ƽ���� ���Ѵ�.

	while (!_TaskQueue.empty())
	{
		queueEvent* jobElem = _TaskQueue.top(); // ���� �켱������ ���;��� �̺�Ʈ�� ���ؼ�
		if (_CurrentTick < jobElem->generateTime) continue; // ���� ȣ���� ������ ���� �ʾ��� ��� ������ ���´�.
		jobElem->Task(); // ���� ȣ���� ������ �ƴٸ� �ش� ���� �����ϰ� queue���� ����
		_TaskQueue.pop();
		delete jobElem;
		
	}

}


void Scheduler::DoNormalTasks()
{
	/// tick update
	while (!_normalQueue.empty())
	{
		queueEvent* jobElem = _normalQueue.front(); // ���� �켱������ ���;��� �̺�Ʈ�� ���ؼ�
		_normalQueue.pop();
		if (jobElem != nullptr)
		{
			jobElem->Task(); // ���� ȣ���� ������ �ƴٸ� �ش� ���� �����ϰ� queue���� ����
			delete jobElem;
		}
	}

}