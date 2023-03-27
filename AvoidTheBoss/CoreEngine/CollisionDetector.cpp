#include "pch.h"
#include "CollisionDetector.h"
#include <math.h>
OcTree* BoxTree = nullptr;


int32 OcTree::_maxLevel = 3;

void OcTree::AddBoundingBox(DirectX::BoundingBox aabb)
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

bool OcTree::CheckCollision(DirectX::BoundingBox& playerBox)
{
		bool rVal = false;
		if (_curLevel == _maxLevel)
		{
			if (_area.Intersects(playerBox))
			{
				std::cout << "collision" << std::endl;
				XMFLOAT3 pCorner[8];
				XMFLOAT3 iCorner[8];

				XMFLOAT3 centerOffset;
				

				for (auto& i : _node->boxs)
				{
					if (i.Intersects(playerBox))
					{
						centerOffset.x = playerBox.Center.x - i.Center.x;
						centerOffset.z = playerBox.Center.z - i.Center.z;
						float offsetX = playerBox.Extents.x + i.Extents.x - ::fabsf(centerOffset.x);
						float offsetZ = playerBox.Extents.z + i.Extents.z - ::fabsf(centerOffset.z);

						if (centerOffset.x > 0) playerBox.Center.x += offsetX;
						else playerBox.Center.x -= offsetX;
						if (centerOffset.z > 0) playerBox.Center.z += offsetZ;
						else playerBox.Center.z -= offsetZ;
						return true;
					}
				}
				return false;
			}
			else return false;
		}
		else if (_curLevel < _maxLevel)
		{
			
			if (_area.Intersects(playerBox))
			{
				for (auto& i : _childTree)
				{
					rVal |= i->CheckCollision(playerBox);
				}
			}
			return rVal;
		}
}