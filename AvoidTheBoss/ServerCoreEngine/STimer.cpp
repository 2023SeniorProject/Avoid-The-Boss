#include "pch.h"
#include "STimer.h"

STimer::STimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&_fTimePerformancePerSec);
	// 현재 타이머의 주파수를 가져온다.
	::QueryPerformanceCounter((LARGE_INTEGER*)&_fTimeLast);
	// 현재 컴퓨터의 cpu의 퍼포먼스 카운트 값을 반환한다.
	_fTimeScale = 1.0 / (double)_fTimePerformancePerSec;

	_fTimeBase = _fTimeLast;
	_fTimePaused = 0;
	_fTimeStop = 0;

	m_nSampleCount = 0;
	_curFramerate = 0;
	_nFramePerSec = 0;
	_totalElapsedTime = 0.0f;
}

STimer::~STimer()
{
}

void STimer::Start()
{
	int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);
	

	if (m_bStopped)
	{
		_fTimePaused += (nPerformanceCounter - _fTimeStop);
		_fTimeLast = nPerformanceCounter;
		_fTimeStop = 0;
		m_bStopped = false;
	}
}

void STimer::Stop()
{
	if (!m_bStopped)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&_fTimeStop);
		m_bStopped = true;
	}
}

void STimer::Reset()
{
	int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	_fTimeBase = nPerformanceCounter;
	_fTimeLast = nPerformanceCounter;
	_fTimeStop = 0;

	m_bStopped = false;
}

void STimer::Tick(float fLockFPS)
{
	if (m_bStopped)
	{
		_fTimeElapsedAvg = 0.0f;
		return;
	}

	//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다. 
	::QueryPerformanceCounter((LARGE_INTEGER*)&_fTimeCurrent);
	float fTimeElapsed = float((_fTimeCurrent - _fTimeLast) * _fTimeScale); // 초단위로 변경
	
	//현재 시간을 m_nLastTime에 저장한다. 
	_fTimeLast = _fTimeCurrent;

	if (fabsf(fTimeElapsed - _fTimeElapsedAvg) < 1.0f) // 오차가 적다면
	{
		// 배열 값들을 한칸 씩 미룬다.
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT2 - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		
		if (m_nSampleCount < MAX_SAMPLE_COUNT2) m_nSampleCount++;
	}
	
	//프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 저장한다. 
	_nFramePerSec++;
	_nWorldFrame++;
	_totalElapsedTime += fTimeElapsed;
	
	if (_totalElapsedTime > 1.0f)
	{
		_curFramerate = _nFramePerSec;
		_nFramePerSec = 0;
		_totalElapsedTime = 0.0f;
	}

	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구한다. 
	_fTimeElapsedAvg = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++) _fTimeElapsedAvg += m_fFrameTime[i];
	
	if (m_nSampleCount > 0)
	{
		(_fTimeElapsedAvg) /= m_nSampleCount;
		//std::cout << _fTimeElapsedAvg << std::endl;
	}
}

unsigned long  STimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 “ FPS”와 결합한다. 
	if (lpszString)
	{
		_itow_s(_curFramerate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}
	return(_curFramerate);
}

float STimer::GetTimeElapsed()
{
	return(_fTimeElapsedAvg);
}

float STimer::GetTotalTime()
{
	if (m_bStopped) return(float(((_fTimeStop - _fTimePaused) - _fTimeBase) * _fTimeScale));
	return(float(((_fTimeCurrent - _fTimePaused) - _fTimeBase) * _fTimeScale));
}
