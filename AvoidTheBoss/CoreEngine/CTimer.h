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

	Clock::time_point		_lastTimePoint; // 마지막 시점 시간
	Clock::time_point		_StopTimePoint; // 멈춘 시점 
	Clock::time_point		_initTimePoint;

	float					_fTimeElapsedAvg = 0.f; // 한 프레임 처리하는데 걸리는 평균 시간
	float					_accumulateElapsedTime = 0.f; // 한 프레임 만큼 처리 됐는지 확인하는 용도
public:
	float					_accumulateFPSLockTime = 0.f; // 월드 프레임 증감을 위한 누적 시간.
	float					_accumulateTimeForHistory = 0.f;
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS); // 타이머 시간 갱신
	void Start();
	void Stop();
	void Reset();
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // 프레임 레이트 반환
	float GetTimeElapsed(); // 프레임 평균 경과 시간 반환
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

