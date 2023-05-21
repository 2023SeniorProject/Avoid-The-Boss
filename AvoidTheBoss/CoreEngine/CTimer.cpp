#include "pch.h"
#include "CTimer.h"
#include <cmath>


Timer::Timer()
{
	_nSampleCount = 0;
	_curFrameRate = 0;
	_nFramePerSec = 0;
	_accumulateElapsedTime = 0.0f;


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
	if (fLockFPS != 0.0f) _fTimeElapsedAvg = (int)((1.f / fLockFPS) * 1000.f); // 60 FPS 기준 16값이 나온다.
	


	if (_bStopped)
	{
		_fTimeElapsedAvg = 0.0f;
		return;
	}

	std::chrono::time_point curTimePoint = Clock::now();
	std::chrono::duration<float, std::milli> fp_ms = (curTimePoint - _lastTimePoint); // 반 올림하지 않고 정확하게 출력하기
	_fTimeElapsed = fp_ms.count();
	_lastTimePoint = curTimePoint;
	
	// 평균 프레임을 샘플링 하기 위한 코드
	if (fabsf(_fTimeElapsed - _fTimeElapsedAvg) < 1000.f) // 오차가 0.1초 미만이라면 샘플링을 허용해준다.
	{
		::memmove(&_SampleFrameTime[1], _SampleFrameTime, ((MAX_SAMPLE_COUNT) - 1) * sizeof(float));
		_SampleFrameTime[0] = _fTimeElapsed;
		
		if (_nSampleCount < MAX_SAMPLE_COUNT) _nSampleCount++;
		
	}

	_accumulateElapsedTime += _fTimeElapsed;
	_accumulateFPSLockTime += _fTimeElapsed;
	_accumulateTimeForHistory += _fTimeElapsed;

	if (fLockFPS != 0.0f && (_fTimeElapsed > _fTimeElapsedAvg))
	{
		_nFramePerSec++;
	}

	if (_accumulateElapsedTime >= 1000.f) // 1초 지나면 끝
	{
		_curFrameRate = _nFramePerSec; // 현재 프레임률
		_nFramePerSec = 0;             // 초당 프레임
		_accumulateElapsedTime = 0.0f; // 총 누적 프레임 (1초 기준)
	}

	
	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구한다. 
	if (fLockFPS == 0.f) // 고정 fps 아니면 평균 출력
	{
		//_fTimeElapsedAvg = 0.0f;
		for (ULONG i = 0; i < _nSampleCount; i++) _fTimeElapsedAvg += _SampleFrameTime[i];
		if (_nSampleCount > 0)
		{
			(_fTimeElapsedAvg) /= _nSampleCount;
		}
		_curFrameRate = _fTimeElapsedAvg;
	}
	GetFrameRate();
}

unsigned long Timer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 “ FPS”와 결합한다. 
	return(_curFrameRate);
}

float Timer::GetTimeElapsed()
{
	return (_fTimeElapsed / 1000.f);
}


float Timer::GetDeltaTime(float fpsLock)
{
	return (1.f / fpsLock);
}