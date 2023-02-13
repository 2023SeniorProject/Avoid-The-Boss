#pragma once

// === RW Spin Lock ===
// ���� �ڿ��� ��� �����尡 �б⸸ ������ ���� Lock ���� ������ �� �ֵ��� �ϰ�, �Ѹ��̶� �����ϰ� �ִٸ�, ���� �ɾ ��������.
// ���� 16��Ʈ ���� 16��Ʈ RWFlag�� ���� ��� �����尡 ���� �ڿ��� �а� ���� �ִ��� �ľ�����. ==> ��Ʈ �÷��� ���
// W �϶� W ����
// W �϶� R ����
// R �϶� W �Ұ���
// === === === === === 

class RWLock
{
	enum RWFlag 
	{
		MAX_WAIT_TICK = 3000, // ���յ��� �ִ������� ��ٸ� �� �ִ� ƽ
		WRITE_FLAG_MASK = 0xFFFF'0000, // & ������ ���� ���� 16��Ʈ�� ������ �� �ִ�.
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
