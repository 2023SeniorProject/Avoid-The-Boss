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
	float _ActiveRadius = 1.0f;

public:
	SwitchInfo() { _pos = XMFLOAT3(0.0f, 0.25f, -50.0f); }
	~SwitchInfo() {}

	void SwitchInterationOn() 
	{
		bool expected = false;
		bool desired = true;
		if (_IsOnInteraction.compare_exchange_strong(expected, desired));
		else std::cout << "Already Interactioning\n";

	}

	void SwitchActivate()
	{
		bool expected = false;
		bool desired = true;
		_IsActive.compare_exchange_strong(expected, desired);
	}
	
	void ResetGuage()
	{
		_lock.lock();
		_curGuage = 0.f;
		_coolTime = 0.f;
		_lock.unlock();
	}
	bool CanInteraction(int32 sid);
	bool UpdateGuage(float elapsedTime);
};