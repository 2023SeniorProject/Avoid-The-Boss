#pragma once

const ULONG MAX_SAMPLE_COUNT2 = 50;

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

	float			m_fFrameTime[MAX_SAMPLE_COUNT2]; 
	ULONG			m_nSampleCount; 

	unsigned long			_curFramerate;
	unsigned long			_nFramePerSec;
public:
	unsigned long			_nWorldFrame;
	float			_totalElapsedTime;

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

