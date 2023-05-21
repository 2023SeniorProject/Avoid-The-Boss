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
	if (fLockFPS != 0.0f) _fTimeElapsedAvg = (int)((1.f / fLockFPS) * 1000.f); // 60 FPS ���� 16���� ���´�.
	


	if (_bStopped)
	{
		_fTimeElapsedAvg = 0.0f;
		return;
	}

	std::chrono::time_point curTimePoint = Clock::now();
	std::chrono::duration<float, std::milli> fp_ms = (curTimePoint - _lastTimePoint); // �� �ø����� �ʰ� ��Ȯ�ϰ� ����ϱ�
	_fTimeElapsed = fp_ms.count();
	_lastTimePoint = curTimePoint;
	
	// ��� �������� ���ø� �ϱ� ���� �ڵ�
	if (fabsf(_fTimeElapsed - _fTimeElapsedAvg) < 1000.f) // ������ 0.1�� �̸��̶�� ���ø��� ������ش�.
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

	if (_accumulateElapsedTime >= 1000.f) // 1�� ������ ��
	{
		_curFrameRate = _nFramePerSec; // ���� �����ӷ�
		_nFramePerSec = 0;             // �ʴ� ������
		_accumulateElapsedTime = 0.0f; // �� ���� ������ (1�� ����)
	}

	
	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�. 
	if (fLockFPS == 0.f) // ���� fps �ƴϸ� ��� ���
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
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�. 
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