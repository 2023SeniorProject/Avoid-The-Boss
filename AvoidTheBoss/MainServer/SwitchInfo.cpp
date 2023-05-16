#include "pch.h"
#include "SwitchInfo.h"
#include "CSIocpCore.h"


void SwitchInfo::UpdateGuage(float elapsedTime)
{
	
	if (_IsOnInteraction && !_IsActive)
	{
		_lock.lock();
		_coolTime -= elapsedTime;
		if (_coolTime <= 0.f)
		{
			
			_coolTime = 0.1;
			_curGuage += _GuageOffset;
			
			std::cout << "["<< _idx << "]" << _curGuage << "%\n";
	
			if (_curGuage == _maxGuage)
			{
				SwitchActivate(true);
			}
		}
		_lock.unlock();
	}
	
}

bool SwitchInfo::CanInteraction(int32 rm,int32 sid)
{
	ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid)._lock.lock();
	XMFLOAT3 myPlayerpos = ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid).GetPosition();
	XMFLOAT3 distance = Vector3::Subtract(myPlayerpos,_pos);
	float range = 0.2 + _ActiveRadius;
	ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid)._lock.unlock();
	if (Vector3::Length(distance) <= range) return true;
	return false;
}