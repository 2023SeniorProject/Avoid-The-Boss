#pragma once
#include <chrono>

const ULONG MAX_SAMPLE_COUNT = 50;

class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
public:
	float					_fTimeDuringPaused;
	bool					_bStopped;

	float					_SampleFrameTime[MAX_SAMPLE_COUNT]; 
	ULONG					_nSampleCount; 

	unsigned long			_curFrameRate;
	unsigned long			_nFramePerSec;

	Clock::time_point		_lastTimePoint; // ������ ���� �ð�
	Clock::time_point		_StopTimePoint; // ���� ���� 
	Clock::time_point		_initTimePoint;

	float					_fTimeElapsedAvg = 0.f; // �� ������ ó���ϴµ� �ɸ��� ��� �ð�
	float					_accumulateElapsedTime = 0.f; // �� ������ ��ŭ ó�� �ƴ��� Ȯ���ϴ� �뵵
public:
	float					_accumulateElapsedTimeForWorldFrame = 0.f; // ���� ������ ������ ���� ���� �ð�.
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	
};

