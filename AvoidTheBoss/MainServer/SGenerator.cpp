#include "pch.h"
#include "SGenerator.h"
#include "CSIocpCore.h"




bool SGenerator::CanInteraction(int32 rm, int32 sid)
{
	CGameManager& gm = ServerIocpCore._rmgr->GetRoom(rm)._gameLogic;
	SPlayer& tp = gm.GetPlayerBySid(sid);
	XMFLOAT3 myPlayerpos = tp.GetPosition();
	XMFLOAT3 distance = Vector3::Subtract(myPlayerpos,_pos);
	float range = 0.5 + _ActiveRadius;
	if (Vector3::Length(distance) <= range) return true;
	return false;
}