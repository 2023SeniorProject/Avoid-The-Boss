#include "pch.h"
#include "CollisionDetector.h"

OcTree* BoxTree = nullptr;


int32 OcTree::_maxLevel = 0;

void OcTree::AddBoundingBox(DirectX::BoundingBox aabb)
{
	if (_curLevel == _maxLevel)
	{
		//if (_area.IsIncludePoint(aabb.Center))
		if (_area.Intersects(aabb))
		{
			std::cout << "[" << _cnt << "]" << "(" << aabb.Center.x << " " << aabb.Center.y << ") ";
			std::cout << "(" << aabb.Extents.x << " " << aabb.Extents.z << ")\n";
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

void OcTree::CheckCollision(DirectX::BoundingBox& aabb)
{
	{
		if (_curLevel == _maxLevel)
		{
			if (_area.Intersects(aabb))
			{
				for (auto& i : _node->boxs)
				{
					if (i.Intersects(aabb))
					{
						std::cout  << "Mbox (" << i.Center.x << "," << i.Center.z << ")\n";
					}
				}
			}
		}
		else if (_curLevel < _maxLevel)
		{
			if (_area.Intersects(aabb))
			{
				for (auto& i : _childTree) i->CheckCollision(aabb);
			}
		}

	}
}