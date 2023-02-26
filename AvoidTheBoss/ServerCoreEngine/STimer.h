#pragma once

const ULONG MAX_SAMPLE_COUNT2 = 50;

class STimer
{
private:
	double			_fTimeScale; 
	float			_fTimeElapsedAvg; // �� ������ ó���ϴµ� �ɸ��� ��� �ð�

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

	void Tick(float fLockFPS = 0.0f); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetTotalTime();
};

