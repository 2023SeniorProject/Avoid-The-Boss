#include "pch.h"
#include "CollisionDetector.h"

int32 OcTree::_maxLevel = 2;

void OcTree::AddBoundingBox(DirectX::BoundingBox& aabb)
{
	if (_curLevel == _maxLevel)
	{
		if (_area.IsIncludePoint(aabb.Center))
		{
			_area.print();
			_node->addBoxs(aabb);
		}
	}
	else if (_curLevel < _maxLevel)
	{
		if (_area.IsIncludePoint(aabb.Center))
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
	centers[0] = { _area._center.x + (_volume / 4),  _area._center.y + (_volume / 4), _area._center.z - (_volume / 4) };
	centers[1] = { _area._center.x - (_volume / 4),  _area._center.y + (_volume / 4), _area._center.z - (_volume / 4) };

	centers[2] = { _area._center.x + (_volume / 4),  _area._center.y + (_volume / 4), _area._center.z + (_volume / 4) };
	centers[3] = { _area._center.x - (_volume / 4),  _area._center.y + (_volume / 4), _area._center.z + (_volume / 4) };

	centers[4] = { _area._center.x + (_volume / 4),  _area._center.y - (_volume / 4), _area._center.z + (_volume / 4) };
	centers[5] = { _area._center.x - (_volume / 4),  _area._center.y - (_volume / 4), _area._center.z + (_volume / 4) };

	centers[6] = { _area._center.x + (_volume / 4),  _area._center.y - (_volume / 4), _area._center.z - (_volume / 4) };
	centers[7] = { _area._center.x - (_volume / 4),  _area._center.y - (_volume / 4), _area._center.z - (_volume / 4) };

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
			if (_area.IsIncludePoint(aabb.Center))
			{
				for (auto& i : _node->boxs)
				{
					if (i.Intersects(aabb))
					{
						std::cout << "collision" << std::endl;

					}
				}
			}
		}
		else if (_curLevel < _maxLevel)
		{
			if (_area.IsIncludePoint(aabb.Center))
			{
				for (auto& i : _childTree) i->CheckCollision(aabb);
			}
		}

	}
}