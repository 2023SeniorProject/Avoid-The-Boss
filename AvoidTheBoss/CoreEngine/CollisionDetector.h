#pragma once

#pragma once
#include <array>

class LeafNode
{
public:
	std::vector<DirectX::BoundingBox> boxs;
public:
	LeafNode() {}
	void addBoxs(DirectX::BoundingBox aabb) { boxs.push_back(aabb); }
	virtual ~LeafNode() { boxs.clear(); }
};

class OcTree
{

	struct Sector
	{
		DirectX::XMFLOAT3 _center;
		DirectX::XMFLOAT3 _leftTopBack;
		DirectX::XMFLOAT3 _rightBottomFront;
	public:
		Sector()
		{

			_center = { 0,0,0 };
		}
		Sector(DirectX::XMFLOAT3 center, float volume)
		{
			_center = center;

		}
		bool IsIncludePoint(const XMFLOAT3 pos)
		{

		}

		void print()
		{

		}
	};

public:
	static int32 _maxLevel;
	int32 _curLevel = 0;
	float _volume = 0;
	OcTree* _parentTree;
	std::array<OcTree*, 8> _childTree;
	LeafNode* _node;
	Sector _area;
public:
	OcTree() { };
	OcTree(DirectX::XMFLOAT3 center, float volume) : _volume(volume)
	{
		if (_curLevel == 0) _parentTree = this;
		_area = Sector(center, volume);

	};
	OcTree(OcTree* parent, float level, XMFLOAT3 center, float volume) : _parentTree(parent), _curLevel(level), _volume(volume)
	{
		_area = Sector(center, _volume);
		if (_curLevel == _maxLevel) _node = new LeafNode();
	};
	void AddBoundingBox(const DirectX::BoundingBox& aabb)
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
				for (auto i : _childTree) i->AddBoundingBox(aabb);
			}
		}
	}
	void BuildTree()
	{
		if (_curLevel < _maxLevel)
		{
			BuildChildTree();
		}
	}
	void BuildChildTree()
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
	void CheckCollision(const DirectX::BoundingBox& aabb)
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
				for (auto i : _childTree) i->CheckCollision(aabb);
			}
		}

	}

};

int32 OcTree::_maxLevel = 3;