#pragma once

// === RW Spin Lock ===
// 공유 자원을 모든 스레드가 읽기만 수행할 때는 Lock 없이 수행할 수 있도록 하고, 한명이라도 점유하고 있다면, 락을 걸어서 수행하자.
// 상위 16비트 하위 16비트 RWFlag를 통해 몇개의 스레드가 공유 자원을 읽고 쓰고 있는지 파악하자. ==> 비트 플래그 사용
// W 일때 W 가능
// W 일때 R 가능
// R 일때 W 불가능
// === === === === === 

class RWLock
{
	enum RWFlag 
	{
		MAX_WAIT_TICK = 3000, // 경합동안 최대한으로 기다릴 수 있는 틱
		WRITE_FLAG_MASK = 0xFFFF'0000, // & 연산을 통해 상위 16비트만 추출할 수 있다.
		READ_FLAG_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};
public:
	void WriteLock();
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();
private:

	Atomic<uint32> _rwFlag = EMPTY_FLAG;
	uint16 _writeCnt = 0;
};

