#include "pch.h"
#include "CTimer.h"
#include <cmath>


Timer::Timer()
{
	_nSampleCount = 0;
	_curFrameRate = 0;
	_nFramePerSec = 0;
	_accumlateElapsedTime = 0.0f;


}

Timer::~Timer()
{
}

void Timer::Start()
{
	std::chrono::time_point resumeTimePoint = Clock::now();
	

	if (_bStopped)
	{
		_fTimeDuringPaused += std::chrono::duration<double>(resumeTimePoint - _StopTimePoint).count();
		_lastTimePoint = resumeTimePoint;
		_bStopped = false;
	}
}

void Timer::Stop()
{
	if (!_bStopped)
	{
		_StopTimePoint = Clock::now();
		_bStopped = true;
	}
}

void Timer::Reset()
{
	_initTimePoint = Clock::now();
	_lastTimePoint = Clock::now();
	_bStopped = false;
}

void Timer::Tick(float fLockFPS)
{
	if (_bStopped)
	{
		_fTimeElapsedAvg = 0.0f;
		return;
	}
	std::chrono::time_point curTimePoint = Clock::now();
	float fTimeElapsed = std::chrono::duration<double>(curTimePoint - _lastTimePoint).count();
	_lastTimePoint = curTimePoint;
	std::cout << fTimeElapsed << std::endl;
	if (fabsf(fTimeElapsed - _fTimeElapsedAvg) < 1.0f) // ������ ���ٸ�
	{
		// �迭 ������ ��ĭ �� �̷��.
		::memmove(&_SampleFrameTime[1], _SampleFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		_SampleFrameTime[0] = fTimeElapsed;
		
		if (_nSampleCount < MAX_SAMPLE_COUNT) _nSampleCount++;
	}
	
	_nFramePerSec++;
	_nWorldFrame++;
	_accumlateElapsedTime += fTimeElapsed;
	
	if (_accumlateElapsedTime > 1.0f)
	{
		_curFrameRate = _nFramePerSec;
		_nFramePerSec = 0;
		_accumlateElapsedTime = 0.0f;
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�. 
	_fTimeElapsedAvg = 0.0f;
	for (ULONG i = 0; i < _nSampleCount; i++) _fTimeElapsedAvg += _SampleFrameTime[i];
	
	if (_nSampleCount > 0)
	{
		(_fTimeElapsedAvg) /= _nSampleCount;
	
	}
	
}

unsigned long  Timer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�. 
	if (lpszString)
	{
		_itow_s(_curFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}
	return(_curFrameRate);
}

float Timer::GetTimeElapsed()
{
	return(_fTimeElapsedAvg);
}

float Timer::GetTotalTime()
{
	std::chrono::time_point _curTimePoint = Clock::now();
	return std::chrono::duration<double>(_curTimePoint - _initTimePoint).count();
}
