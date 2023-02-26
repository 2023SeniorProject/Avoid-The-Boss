#include "pch.h"
#include "STimer.h"

STimer::STimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&_fTimePerformancePerSec);
	// ���� Ÿ�̸��� ���ļ��� �����´�.
	::QueryPerformanceCounter((LARGE_INTEGER*)&_fTimeLast);
	// ���� ��ǻ���� cpu�� �����ս� ī��Ʈ ���� ��ȯ�Ѵ�.
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

	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�. 
	::QueryPerformanceCounter((LARGE_INTEGER*)&_fTimeCurrent);
	float fTimeElapsed = float((_fTimeCurrent - _fTimeLast) * _fTimeScale); // �ʴ����� ����
	
	//���� �ð��� m_nLastTime�� �����Ѵ�. 
	_fTimeLast = _fTimeCurrent;

	if (fabsf(fTimeElapsed - _fTimeElapsedAvg) < 1.0f) // ������ ���ٸ�
	{
		// �迭 ������ ��ĭ �� �̷��.
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT2 - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		
		if (m_nSampleCount < MAX_SAMPLE_COUNT2) m_nSampleCount++;
	}
	
	//������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�. 
	_nFramePerSec++;
	_nWorldFrame++;
	_totalElapsedTime += fTimeElapsed;
	
	if (_totalElapsedTime > 1.0f)
	{
		_curFramerate = _nFramePerSec;
		_nFramePerSec = 0;
		_totalElapsedTime = 0.0f;
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�. 
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
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�. 
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
