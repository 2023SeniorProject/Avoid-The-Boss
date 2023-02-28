#pragma once
#include <chrono>

const ULONG MAX_SAMPLE_COUNT = 50;

class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
public:

	int32					_fTimeElapsedAvg; // 한 프레임 처리하는데 걸리는 평균 시간
	int32					_fTimeDuringPaused;
	

	int32					_SampleFrameTime[MAX_SAMPLE_COUNT]; 
	ULONG					_nSampleCount; 

	unsigned long			_curFrameRate;
	unsigned long			_nFramePerSec;

	Clock::time_point		_lastTimePoint; // 마지막 시점 시간
	Clock::time_point		_StopTimePoint; // 멈춘 시점 
	Clock::time_point		_initTimePoint;
public:
	uint64					_nWorldFrame;
	int32					_accumlateElapsedTime;

	bool					_bStopped;
public:
	Timer();
	virtual ~Timer();

	void Tick(float fLockFPS = 0.0f); // 타이머 시간 갱신
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // 프레임 레이트 반환
	float GetTimeElapsed(); // 프레임 평균 경과 시간 반환
	float GetTotalTime();
};

