#pragma once

#define MAX_SAMPLE_COUNT 50

class STimer
{
private:
	double			_fTimeScale; 
	float			_fTimeElapsedAvg; // 한 프레임 처리하는데 걸리는 평균 시간

	int64			_fTimeBase;
	int64			_fTimePaused;
	int64			_fTimeStop;
	int64			_fTimeCurrent;
	int64			_fTimeLast;

	int64			_fTimePerformancePerSec; 

	uint64			m_fFrameTime[MAX_SAMPLE_COUNT]; 
	ULONG			m_nSampleCount; 

	uint64			_curFramerate; 
	uint64			_nFramePerSec; 
	uint64			_nWorldFrame;
	uint64			_totalElapsedTime; 

	bool			m_bStopped;
public:
	STimer();
	virtual ~STimer();

	void Tick(float fLockFPS = 0.0f); // 타이머 시간 갱신
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // 프레임 레이트 반환
	float GetTimeElapsed(); // 프레임 평균 경과 시간 반환
	float GetTotalTime();
};

