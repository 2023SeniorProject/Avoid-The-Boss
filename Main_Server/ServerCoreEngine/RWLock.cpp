#include "pch.h"
#include "RWLock.h"
#include "CoreTLS.h"
#include <chrono>

#define MAX_SPIN_CNT 3000

void RWLock::WriteLock()
{
	// 아무도 공유 자원 소유 X 
	// 만약 동일한 스레드가 재귀적으로 lock을 잡는 경우, 무조건 성공하도록 한다.
	const uint32 curThreadId = (_rwFlag.load() & WRITE_FLAG_MASK) >> 16;
	if (lThreadId == curThreadId)
	{
		++_writeCnt;
		return;
	}

	const uint32 desired = (WRITE_FLAG_MASK & lThreadId << 16); // 스레드id(상위 16비트)  0000 0000
	uint32 expected = EMPTY_FLAG;

	auto begin = chrono::high_resolution_clock::now();
	while (true)
	{	
		for (uint32 spinCnt = 0; spinCnt < MAX_SPIN_CNT; ++spinCnt) // tick 동안 못할 경우 소유권을 내려놓고 나중에 다시 와라
		{
			if (_rwFlag.compare_exchange_strong(OUTPUT expected, INPUT desired))
			{
				++_writeCnt; // 재귀적으로 lock을 걸 수 있음.
				return;
			} // empty 일때만 하고~ 
		}
		auto end = chrono::high_resolution_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(begin - end).count() > MAX_WAIT_TICK) CRASH("TIME OUT");
		this_thread::yield();
	}
}

void RWLock::WriteUnLock()
{
	// Read 상태에서 W를 시도할 경우
	if ((_rwFlag.load() & READ_FLAG_MASK) != 0) CRASH("INVALID UNLOCK ORDER"); // 크래시 발생

	const uint32 cnt = _writeCnt -= 1;
	if (cnt == 0)
		_rwFlag.store(EMPTY_FLAG);

}

void RWLock::ReadLock()
{
	// 동일한 스레드가 read를 시도하면 그냥 read flag에 1을 더해준다.
	const uint32 curThreadId = (_rwFlag.load() & WRITE_FLAG_MASK) >> 16;
	if (lThreadId == curThreadId)
	{
		_rwFlag.fetch_add(1);
		return;
	}

	// 누가 w 하고 있지 않는다면 무조건 성공
	auto begin = chrono::high_resolution_clock::now();
	while (true)
	{
		for (uint32 spinCnt = 0; spinCnt < MAX_SPIN_CNT; ++spinCnt) // tick 동안 못할 경우 소유권을 내려놓고 나중에 다시 와라
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

// RWLock을 위한 LockGuard

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