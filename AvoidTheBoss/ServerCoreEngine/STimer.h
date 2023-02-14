#pragma once

#define MAX_SAMPLE_COUNT 50

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

	void Tick(float fLockFPS = 0.0f); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetTotalTime();
};

