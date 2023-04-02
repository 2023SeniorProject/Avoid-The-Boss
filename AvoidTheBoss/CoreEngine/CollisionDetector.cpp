#include "pch.h"
#include "CollisionDetector.h"

OcTree* BoxTree = nullptr;


int32 OcTree::_maxLevel = 3;



float DotProduct(XMFLOAT3 point, XMFLOAT3 axis)
{
	return point.x * axis.x + point.y * axis.y + point.z * axis.z;

}

XMFLOAT3 Project(XMFLOAT3 center, XMFLOAT3 axis, XMFLOAT3 extents) {
	XMFLOAT3 projection;
	float projectionLength = DotProduct(center, axis);
	projection.x = projectionLength + extents.x * ::abs(axis.x);
	projection.y = projectionLength + extents.y * ::abs(axis.y);
	projection.z = projectionLength + extents.z * ::abs(axis.z);
	return projection;
}


float overlap(XMFLOAT3 projA, XMFLOAT3 projB, XMFLOAT3 axis) 
{
	float dist = abs(DotProduct(XMFLOAT3(projA.x - projB.x , projA.y - projB.y, projA.z - projB.z), axis));
	float totalExtent = projA.x + projA.y + projA.z + projB.x + projB.y + projB.z;
	return totalExtent - 2.0f * dist;
}

void OcTree::AddBoundingBox(DirectX::BoundingOrientedBox aabb)
{
	if (_curLevel == _maxLevel)
	{
		if (_area.Intersects(aabb))
		{
			_node->addBoxs(aabb);
		}
	}
	else if (_curLevel < _maxLevel)
	{
		if (_area.Intersects(aabb))
		{
			for (auto& i : _childTree) i->AddBoundingBox(aabb);
		}
	}
}

void OcTree::BuildTree()
{
	if(_curLevel < _maxLevel) BuildChildTree();
}

void OcTree::BuildChildTree()
{
	XMFLOAT3 centers[8];
	centers[0] = { _center.x + (_volume / 4),  _center.y + (_volume / 4), _center.z - (_volume / 4) };
	centers[1] = { _center.x - (_volume / 4),  _center.y + (_volume / 4), _center.z - (_volume / 4) };
	centers[2] = { _center.x + (_volume / 4),  _center.y + (_volume / 4), _center.z + (_volume / 4) };
	centers[3] = { _center.x - (_volume / 4),  _center.y + (_volume / 4), _center.z + (_volume / 4) };
	centers[4] = { _center.x + (_volume / 4),  _center.y - (_volume / 4), _center.z + (_volume / 4) };
	centers[5] = { _center.x - (_volume / 4),  _center.y - (_volume / 4), _center.z + (_volume / 4) };
	centers[6] = { _center.x + (_volume / 4),  _center.y - (_volume / 4), _center.z - (_volume / 4) };
	centers[7] = { _center.x - (_volume / 4),  _center.y - (_volume / 4), _center.z - (_volume / 4) };

	for (int i = 0; i < 8; ++i)
	{
		_childTree[i] = new OcTree(this, _curLevel + 1, centers[i], (_volume / 2));
		_childTree[i]->BuildTree();
	}
}

bool OcTree::CheckCollision(DirectX::BoundingOrientedBox& playerBox, XMFLOAT3& look, XMFLOAT3& right, XMFLOAT3& up)
{
		bool rVal = false;
		if (_curLevel == _maxLevel)
		{
			if (_area.Intersects(playerBox))
			{
				bool rVal2 = false;
				
				
				for (auto& i : _node->boxs)
				{
					if (i.Intersects(playerBox))
					{
						
						// To Do Collision Response

						// 1. 자기의 로컬 좌표계 기준으로 min max 값을 구한다.
						// 플레이어의 자기 로컬 좌표계에 투영했을 때 가장 큰 좌표값과 가장 작은 좌표값을 구한다.
						// look = z
						// up = y
						// right = x
						XMFLOAT3 iCenterInPlayerAxis;
						iCenterInPlayerAxis.x = i.Center.x - playerBox.Center.x;
						iCenterInPlayerAxis.y = i.Center.y - playerBox.Center.y;
						iCenterInPlayerAxis.z = i.Center.z - playerBox.Center.z;

			
						float CenterDistX = DotProduct(iCenterInPlayerAxis, right);
						float CenterDistZ = DotProduct(iCenterInPlayerAxis, look);
						
						float extentDistX = DotProduct(i.Extents, right);
						float extentDistZ = DotProduct(i.Extents, look);

						float xover = (::fabs(extentDistX) + playerBox.Extents.x) - ::fabs(CenterDistX);
						float zover = (::fabs(extentDistZ) + playerBox.Extents.z) - ::fabs(CenterDistZ);

						if (zover > xover)
						{
							if (iCenterInPlayerAxis.x >= 0) playerBox.Center.x -= xover * 1.3f;
							else playerBox.Center.x += xover * 1.3f;
						}
						if (xover > zover)
						{
							if (iCenterInPlayerAxis.z >= 0) playerBox.Center.z -= zover * 1.3f;
							else playerBox.Center.z += zover * 1.3f;
						}
						
						return rVal2 |= true;
					}
				}

				return rVal2;
			}
			else return false;
		}
		else if (_curLevel < _maxLevel)
		{
			
			if (_area.Intersects(playerBox))
			{
				for (auto& i : _childTree)
				{
					rVal |= i->CheckCollision(playerBox,look,right,up);
				}
			}
			return rVal;
		}
}