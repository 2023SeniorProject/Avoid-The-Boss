#include "pch.h"
#include "SwitchInfo.h"
#include "IocpCore.h"


bool SwitchInfo::UpdateGuage(float elapsedTime)
{
		if (_IsOnInteraction)
		{
			_coolTime -= elapsedTime;
			if (_coolTime <= 0.f)
			{
				_lock.lock();
				_coolTime = 16;
				_curGuage += _GuageOffset;
				_lock.unlock();
			}
			else return false;

			if (_curGuage = _maxGuage)
			{
				SwitchActivate();
				return true;
			}
		}

		return false;
}

bool SwitchInfo::CanInteraction(int32 sid)
{
	ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid)._lock.lock();
	XMFLOAT3 distance = Vector3::Subtract(ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid).GetPosition(),_pos);
	ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid)._lock.unlock();

	if (Vector3::Length(distance) <= _ActiveRadius) return true;
	return false;
}