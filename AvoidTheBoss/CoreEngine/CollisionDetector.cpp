#include "pch.h"
#include "CollisionDetector.h"
OcTree* BoxTree = nullptr;


int32 OcTree::_maxLevel = 4;



float DotProduct(XMFLOAT3 a, XMFLOAT3 b) 
{
	return  a.x * b.x + a.y + b.y + a.z * b.z;
}

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

float clamp(float pos, float min, float max)
{
	float val = (pos < min ? min : pos);
	val = val > max ? max : val;
	if (val != min && val != max) return ((val - min) > (max - val) ? max : min);
	else return val;
}
bool OcTree::CheckCollision(DirectX::BoundingSphere& playerBox)
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

					// 1. �ڱ��� ���� ��ǥ�� �������� min max ���� ���Ѵ�.
					// �÷��̾��� �ڱ� ���� ��ǥ�迡 �������� �� ���� ū ��ǥ���� ���� ���� ��ǥ���� ���Ѵ�.
					// look = z
					// up = y
					// right = x
					XMFLOAT3 centerVec{ playerBox.Center.x - i.Center.x, 0.f , playerBox.Center.z - i.Center.z }; // ���� �߽����� �÷��̾� ������� ��ġ ����.

					//// ������ �簢�� ���� �� ���� �߽ɰ� ���� ����� ������ x , z ��ǥ�� ���Ѵ�.
					float closeX = clamp(centerVec.x, -1 * i.Extents.x, i.Extents.x);
					float closeZ = clamp(centerVec.z, -1 * i.Extents.z, i.Extents.z);

					//// �÷��̾� ��ǥ�������� ��ŭ �Ÿ����� ���Ѵ�.
					XMFLOAT3 closeDist{ centerVec.x - closeX , 0.f , centerVec.z - closeZ }; // ���� ���
				
					

					if (::fabs(playerBox.Radius - closeDist.x) < ::fabs(playerBox.Radius - closeDist.z)) // offset ��ġ�� ���� ������ ���.
					{
						playerBox.Center.x += ::fabs((playerBox.Radius - closeDist.x)) * 1.2f;
					}
					else
					{
						playerBox.Center.z += ::fabs((playerBox.Radius - closeDist.z)) * 1.2f;
					}
					
					rVal2 |= true;
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
				for (auto& i : _childTree)
				{
					rVal |= i->CheckCollision(playerBox);
				}
				rVal |= i->CheckCollision(playerBox);
			}
		}
		return rVal;
	}
}
