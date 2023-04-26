#include "pch.h"
#include "SwitchInfo.h"
#include "IocpCore.h"


bool SwitchInfo::UpdateGuage(float elapsedTime)
{
	if (_IsOnInteraction && !_IsActive)
	{
		_coolTime -= elapsedTime;
		if (_coolTime <= 0.f)
		{
			_lock.lock();
			_coolTime = 100.f / 1000.f;
			_curGuage += _GuageOffset;
			_lock.unlock();
			std::cout << "["<< _idx << "]" << _curGuage << "%\n";
	
			if (_curGuage == _maxGuage)
			{
				SwitchActivate();
				return true;
			}
		}
	}

	return false;
}

bool SwitchInfo::CanInteraction(int32 sid)
{
	ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid)._lock.lock();
	XMFLOAT3 myPlayerpos = ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid).GetPosition();
	XMFLOAT3 distance = Vector3::Subtract(myPlayerpos,_pos);
	std::cout << myPlayerpos.x << " " << myPlayerpos.z << "\n";
	float range = 0.2 + _ActiveRadius;
	ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid)._lock.unlock();
	if (Vector3::Length(distance) <= range) return true;
	return false;
}