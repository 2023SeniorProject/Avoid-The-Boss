#include "pch.h"
#include "CollisionDetector.h"

OcTree* BoxTree = nullptr;


int32 OcTree::_maxLevel = 3;


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

float clamp(float pos, float min , float max)
{
	float val = (pos < min ? min : pos);
	val =  val > max ? max : val;
	if (val != min && val != max) return ((val - min) > (max - val) ? max : min);
	else return val;
}
bool OcTree::CheckCollision(DirectX::BoundingSphere& playerBox, XMFLOAT3& look, XMFLOAT3& right, XMFLOAT3& up)
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
						
						// ������ �簢�� ���� �� ���� �߽ɰ� ���� ����� ������ x , z ��ǥ�� ���Ѵ�.
						float closeX = clamp(centerVec.x,  -1 * i.Extents.x, i.Extents.x);
						float closeZ = clamp(centerVec.z,  -1 * i.Extents.z, i.Extents.z);
						
						// �÷��̾� ��ǥ�������� ��ŭ �Ÿ����� ���Ѵ�.
						XMFLOAT3 closeDist { centerVec.x - closeX , 0.f , centerVec.z - closeZ }; // ���� ���
						
						//float cd = Vector3::Length(closeDist); // �Ÿ� ���
						//float offsetdist = ::fabs(playerBox.Radius - cd); // offset �Ÿ� ���. ���� ������ - ���� ����� ���� ���� �߽��� �Ÿ�

						//XMFLOAT3 a = Vector3::ScalarProduct(closeDist, offsetdist, true); // �ش� �������� ��Į���
						if (::fabs(playerBox.Radius - closeDist.x) < ::fabs(playerBox.Radius - closeDist.z)) // offset ��ġ�� ���� ������ ���.
						{
							playerBox.Center.x += (playerBox.Radius - closeDist.x);
						}
						else
						{
							 playerBox.Center.z += (playerBox.Radius - closeDist.z);
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
					rVal |= i->CheckCollision(playerBox,look,right,up);
				}
			}
			return rVal;
		}
}