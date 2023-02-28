#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CTimer
{
public:
	unsigned long	m_nWorldFrame;   //�ʴ� ������ 
private:
	double			m_fTimeScale; //Scale Counter�� �� 1�� ������ �ٲٱ� ���ؼ� ����
	float			m_fTimeElapsed; // ��� ������ ó�� �ð� 1 / Frame ��

	__int64			m_nBasePerformanceCounter;
	__int64			m_nPausedPerformanceCounter;
	__int64			m_nStopPerformanceCounter;
	__int64			m_nCurrentPerformanceCounter;
	__int64			m_nLastPerformanceCounter;

	__int64			m_nPerformanceFrequencyPerSec; //��ǻ���� Performance Frequency

	float			m_fFrameTime[MAX_SAMPLE_COUNT]; //������ �ð��� �����ϱ� ���� �迭
	ULONG			m_nSampleCount; //���ø� �� ������ ���� 

	unsigned long	m_nCurrentFrameRate;  //1�� ���� ���� ������ ��
	unsigned long	m_nFramesPerSecond;   //�ʴ� ������
	
	float			m_fFPSTimeElapsed;    //������ ����Ʈ ��� �ҿ� �ð�

	bool			m_bStopped;
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float fLockFPS = 0.0f); // Ÿ�̸� �ð� ����
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // ������ ����Ʈ ��ȯ
	float GetTimeElapsed(); // ������ ��� ��� �ð� ��ȯ
	float GetTotalTime();
};
