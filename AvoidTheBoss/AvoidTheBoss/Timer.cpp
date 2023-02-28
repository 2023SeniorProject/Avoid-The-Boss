#include "pch.h"
#include "Timer.h"

CTimer::CTimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastPerformanceCounter);
	m_fTimeScale = 1.0 / (double)m_nPerformanceFrequencyPerSec;

	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
	m_nPausedPerformanceCounter = 0;
	m_nStopPerformanceCounter = 0;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CTimer::~CTimer()
{
}

void CTimer::Start()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);
	if (m_bStopped)
	{
		m_nPausedPerformanceCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		m_bStopped = false;
	}
}

void CTimer::Stop()
{
	if (!m_bStopped)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nStopPerformanceCounter);
		m_bStopped = true;
	}
}

void CTimer::Reset()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nBasePerformanceCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;

	m_bStopped = false;
}

void CTimer::Tick(float fLockFPS)
{
	if (m_bStopped)
	{
		m_fTimeElapsed = 0.0f;
		return;
	}

	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�. 
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);
	float fTimeElapsed = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);
	
	//���� �ð��� m_nLastTime�� �����Ѵ�. 
	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������ ���� ������ ó�� �ð�
�� m_fFrameTime[0]�� �����Ѵ�. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f) // ������ ���ٸ�
	{
		// �迭 ������ ��ĭ �� �̷��.
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ�	�����Ѵ�. 
	m_nFramesPerSecond++;
	m_nWorldFrame++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f) // 1�ʰ� �Ѿ�� ������ ī��Ʈ 0
	{
		
		m_nCurrentFrameRate = m_nFramesPerSecond; // 60fps ��� �󸶳� �����°�?
		m_nFramesPerSecond = 0; //���� �������� ��� �����ϵ��� �Ѵ�.
		m_fFPSTimeElapsed = 0.0f;
	}
	
	
	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�. 
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++) m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

unsigned long  CTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�. 
	if (lpszString)
	{
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}
	return(m_nCurrentFrameRate);
}

float CTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}

float CTimer::GetTotalTime()
{
	if (m_bStopped) return(float(((m_nStopPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale));
	return(float(((m_nCurrentPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale));
}