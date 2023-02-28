#pragma once
#include <chrono>

const ULONG MAX_SAMPLE_COUNT = 50;

class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
public:

	int32					_fTimeElapsedAvg; // �� ������ ó���ϴµ� �ɸ��� ��� �ð�
	int32					_fTimeDuringPaused;
	

	int32					_SampleFrameTime[MAX_SAMPLE_COUNT]; 
	ULONG					_nSampleCount; 

	unsigned long			_curFrameRate;
	unsigned long			_nFramePerSec;

	Clock::time_point		_lastTimePoint; // ������ ���� �ð�
	Clock::time_point		_StopTimePoint; // ���� ���� 
	Clock::time_point		_initTimePoint;
public:
	uint64					_nWorldFrame;
	int32					_accumlateElapsedTime;

	bool					_bStopped;
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS = 0.0f); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetTotalTime();
};

