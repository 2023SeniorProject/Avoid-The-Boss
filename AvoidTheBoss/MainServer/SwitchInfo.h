#pragma once

class SwitchInfo
{
public:
	XMFLOAT3 _pos;
	Atomic<bool> _IsOnInteraction = false;
	Atomic<bool> _IsActive = false;
	float _maxGuage = 100;
	float _curGuage = 0.f;
	float _GuageOffset = 1.0f;
	std::mutex _lock;
	float _coolTime;
	float _ActiveRadius = 1.25f;
	int32 _idx;
public:
	SwitchInfo() 
	{
		_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		_idx = 0;
	}
	~SwitchInfo() {}

	void SwitchInteractionOn(bool value) 
	{
		_IsOnInteraction = value;
	}

	void SwitchActivate(bool value)
	{
		
		_IsActive = value;
		
	}
	
	void ResetGuage()
	{
		_lock.lock();
		_curGuage = 0.f;
		_coolTime = 0.f;
		_lock.unlock();
	}
	bool CanInteraction(int32 sid);
	void UpdateGuage(float elapsedTime);
};