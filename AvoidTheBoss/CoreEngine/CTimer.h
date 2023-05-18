#pragma once
#include <chrono>

const int32 MAX_SAMPLE_COUNT = 50;
const int32 DEAD_RECORNING_TPS = 30;


class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
public:
	float					_fTimeDuringPaused;
	float					_fTimeElapsed;
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
	float					_accumulateFPSLockTime = 0.f; // ���� ������ ������ ���� ���� �ð�.
	float					_accumulateTimeForHistory = 0.f;
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetDeltaTime(float fpsLock);
	bool IsAfterTick(float fpsLock) 
	{ 
		if (_accumulateFPSLockTime > (int)(1 / fpsLock) * 1000.f)
		{
			//std::cout << _accumulateElapsedTime << "\n";
			_accumulateFPSLockTime = 0.f;
			return true;
		}
		else return false; 
	}
	bool IsTimeToAddHistory()
	{
		if (_accumulateTimeForHistory >= 16)
		{
			_accumulateTimeForHistory;
			return true;
		}
		else return false;
	}
};

