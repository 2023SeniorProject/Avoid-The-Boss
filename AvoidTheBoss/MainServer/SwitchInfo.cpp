#include "pch.h"
#include "SwitchInfo.h"
#include "CSIocpCore.h"




bool SGenerator::CanInteraction(int32 rm,int32 sid)
{
	ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid)._lock.lock();
	XMFLOAT3 myPlayerpos = ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid).GetPosition();
	XMFLOAT3 distance = Vector3::Subtract(myPlayerpos,_pos);
	float range = 0.2 + _ActiveRadius;
	ServerIocpCore._rmgr->GetRoom(rm).GetMyPlayerFromRoom(sid)._lock.unlock();
	if (Vector3::Length(distance) <= range) return true;
	return false;
}