#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

class CTimer
{
private:
	double			m_fTimeScale; //Scale Counter의 양 1초 단위로 바꾸기 위해서 생성
	float			m_fTimeElapsed; // 평균 프레임 처리 시간 1 / Frame 초

	__int64			m_nBasePerformanceCounter;
	__int64			m_nPausedPerformanceCounter;
	__int64			m_nStopPerformanceCounter;
	__int64			m_nCurrentPerformanceCounter;
	__int64			m_nLastPerformanceCounter;

	__int64			m_nPerformanceFrequencyPerSec; //컴퓨터의 Performance Frequency

	float			m_fFrameTime[MAX_SAMPLE_COUNT]; //프레임 시간을 누적하기 위한 배열
	ULONG			m_nSampleCount; //샘플링 할 프레임 개수 

	unsigned long	m_nCurrentFrameRate;  //1초 동안 나온 프레임 수
	unsigned long	m_nFramesPerSecond;   //초당 프레임
	unsigned long	m_nWorldFrame;   //초당 프레임 
	float			m_fFPSTimeElapsed;    //프레임 레이트 계산 소요 시간

	bool			m_bStopped;
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float fLockFPS = 0.0f); // 타이머 시간 갱신
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // 프레임 레이트 반환
	float GetTimeElapsed(); // 프레임 평균 경과 시간 반환
	float GetTotalTime();
};

