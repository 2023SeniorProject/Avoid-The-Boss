#include "pch.h"
#include "CJobQueue.h"

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

	while (!_TaskQueue.empty())
	{
		_CurrentTick = GetCurrentTick(); // ���� ƽ���� ���Ѵ�.
		queueEvent* jobElem = _TaskQueue.top(); // ���� �켱������ ���;��� �̺�Ʈ�� ���ؼ�
		if (_CurrentTick < jobElem->generateTime)
		{
			std::cout << _CurrentTick << " | " << jobElem->generateTime << " Not Yet\n";
			// ���ϰ� �ٽ� ����ִ´�.
			_TaskQueue.pop();
			_TaskQueue.push(jobElem);
			continue; // ���� ȣ���� ������ ���� �ʾ��� ��� ������ ���´�.
		}
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