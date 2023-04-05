#include "pch.h"
#include "JobQueue.h"

Scheduler::Scheduler()
{
	_BeginTickPoint = Clock::now(); // ���� ������ ����
	_CurrentTick = GetCurrentTick(); // ���� ƽ �� �ʱ�ȭ
}

void Scheduler::PushTask(queueEvent* task)
{
	int64 dueTimeTick = _CurrentTick;
	task->generateTime = dueTimeTick;
	_TaskQueue.push(task);
}

void Scheduler::DoTasks()
{
	/// tick update
	_CurrentTick = GetCurrentTick(); // ���� ƽ���� ���Ѵ�.

	while (!_TaskQueue.empty())
	{
		queueEvent* jobElem = _TaskQueue.top(); // ���� �켱������ ���;��� �̺�Ʈ�� ���ؼ�
		if (_CurrentTick < jobElem->generateTime) break; // ���� ȣ���� ������ ���� �ʾ��� ��� ������ ���´�.
		jobElem->Task(); // ���� ȣ���� ������ �ƴٸ� �ش� ���� �����ϰ� queue���� ����
		_TaskQueue.pop();
	}
}