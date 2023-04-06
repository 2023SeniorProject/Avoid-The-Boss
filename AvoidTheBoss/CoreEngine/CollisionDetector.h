#pragma once

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
			_leftTopBack = XMFLOAT3(_center.x - (volume / 2), _center.y + (volume / 2), _center.z - (volume / 2));
			_rightBottomFront = XMFLOAT3(_center.x + (volume / 2), _center.y - (volume / 2), _center.z + (volume / 2));

		}
		bool IsIncludePoint(const XMFLOAT3 pos)
		{
			return ((_leftTopBack.x <= pos.x && pos.x <= _rightBottomFront.x) && (_leftTopBack.y >= pos.y && pos.y >= _rightBottomFront.y) && (_leftTopBack.z <= pos.z && pos.z <= _rightBottomFront.z));
		}
		void print()
		{
			//std::cout << "center : " << _center.x << " " << _center.y << " " << _center.z << std::endl;
		
		}
	};

public:
	static int32 _maxLevel;
	static int32 _idx;
	int32 _curLevel = 0;
	float _volume = 0;
	OcTree* _parentTree;
	std::array<OcTree*, 8> _childTree;
	LeafNode* _node;
	
	DirectX::BoundingBox _area;
	DirectX::XMFLOAT3 _center;
	Atomic<int32> _cnt = 0 ;
public:
	OcTree() { };
	OcTree(DirectX::XMFLOAT3 center, float volume) : _volume(volume)
	{
		_center = center;
		_area.Center = center;
		_area.Extents = XMFLOAT3(volume / 2 , volume / 2, volume / 2);
		if (_curLevel == 0) _parentTree = this;
		if (_curLevel == _maxLevel) _node = new LeafNode();
	};
	OcTree(OcTree* parent, float level, XMFLOAT3 center, float volume) : _parentTree(parent), _curLevel(level), _volume(volume)
	{
		_center = center;
		_area.Center = center;
		_area.Extents = XMFLOAT3(volume / 2, volume / 2, volume / 2);

		if (_curLevel == _maxLevel) _node = new LeafNode();
	};
	void AddBoundingBox(DirectX::BoundingBox aabb);
	void BuildTree();
	void BuildChildTree();
	bool CheckCollision(DirectX::BoundingSphere& playerBox, XMFLOAT3& look, XMFLOAT3& right, XMFLOAT3& up);
};

extern class OcTree* BoxTree;

