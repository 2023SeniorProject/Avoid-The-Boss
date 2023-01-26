#include "pch.h"
#include "RWLock.h"
#include "CoreTLS.h"
#include <chrono>

#define MAX_SPIN_CNT 3000

void RWLock::WriteLock()
{
	// �ƹ��� ���� �ڿ� ���� X 
	// ���� ������ �����尡 ��������� lock�� ��� ���, ������ �����ϵ��� �Ѵ�.
	const uint32 curThreadId = (_rwFlag.load() & WRITE_FLAG_MASK) >> 16;
	if (lThreadId == curThreadId)
	{
		++_writeCnt;
		return;
	}

	const uint32 desired = (WRITE_FLAG_MASK & lThreadId << 16); // ������id(���� 16��Ʈ)  0000 0000
	uint32 expected = EMPTY_FLAG;

	auto begin = chrono::high_resolution_clock::now();
	while (true)
	{	
		for (uint32 spinCnt = 0; spinCnt < MAX_SPIN_CNT; ++spinCnt) // tick ���� ���� ��� �������� �������� ���߿� �ٽ� �Ͷ�
		{
			if (_rwFlag.compare_exchange_strong(OUTPUT expected, INPUT desired))
			{
				++_writeCnt; // ��������� lock�� �� �� ����.
				return;
			} // empty �϶��� �ϰ�~ 
		}
		auto end = chrono::high_resolution_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(begin - end).count() > MAX_WAIT_TICK) CRASH("TIME OUT");
		this_thread::yield();
	}
}

void RWLock::WriteUnLock()
{
	// Read ���¿��� W�� �õ��� ���
	if ((_rwFlag.load() & READ_FLAG_MASK) != 0) CRASH("INVALID UNLOCK ORDER"); // ũ���� �߻�

	const uint32 cnt = _writeCnt -= 1;
	if (cnt == 0)
		_rwFlag.store(EMPTY_FLAG);

}

void RWLock::ReadLock()
{
	// ������ �����尡 read�� �õ��ϸ� �׳� read flag�� 1�� �����ش�.
	const uint32 curThreadId = (_rwFlag.load() & WRITE_FLAG_MASK) >> 16;
	if (lThreadId == curThreadId)
	{
		_rwFlag.fetch_add(1);
		return;
	}

	// ���� w �ϰ� ���� �ʴ´ٸ� ������ ����
	auto begin = chrono::high_resolution_clock::now();
	while (true)
	{
		for (uint32 spinCnt = 0; spinCnt < MAX_SPIN_CNT; ++spinCnt) // tick ���� ���� ��� �������� �������� ���߿� �ٽ� �Ͷ�
		{
			uint32 expected = (_rwFlag.load() & READ_FLAG_MASK); 
			if (_rwFlag.compare_exchange_strong(expected, expected + 1)) return;
		}
		auto end = chrono::high_resolution_clock::now();
		if ((chrono::duration_cast<chrono::milliseconds>(end - begin).count()) > MAX_WAIT_TICK) CRASH("TIME OUT");
		this_thread::yield();
	}
}

void RWLock::ReadUnLock()
{
	if ((_rwFlag.fetch_sub(1) & READ_FLAG_MASK) == 0) CRASH("MULTIPLE UNLOCK");

}

// RWLock�� ���� LockGuard

class rlock_guard
{
public:
	rlock_guard(RWLock& lock) : _lock(lock) { _lock.ReadLock(); }
	~rlock_guard() { _lock.ReadUnLock(); }
private:
	RWLock& _lock;
};

class wlock_guard
{
public:
	wlock_guard(RWLock& lock) : _lock(lock) { _lock.WriteLock(); }
	~wlock_guard() { _lock.WriteUnLock(); }
private:
	RWLock& _lock;
};