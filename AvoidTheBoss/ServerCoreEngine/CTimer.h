#pragma once
#include <chrono>

const ULONG MAX_SAMPLE_COUNT = 50;

class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
public:

	float					_fTimeElapsedAvg; // �� ������ ó���ϴµ� �ɸ��� ��� �ð�
	float					_fTimeDuringPaused;
	

	float					_SampleFrameTime[MAX_SAMPLE_COUNT]; 
	ULONG					_nSampleCount; 

	unsigned long			_curFrameRate;
	unsigned long			_nFramePerSec;

	Clock::time_point		_lastTimePoint; // ������ ���� �ð�
	Clock::time_point		_StopTimePoint; // ���� ���� 
	Clock::time_point		_initTimePoint;
public:
	float					_accumlateElapsedTime;
	//float					_nWorldFrame;

	bool					_bStopped;
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetTotalTime();
};

