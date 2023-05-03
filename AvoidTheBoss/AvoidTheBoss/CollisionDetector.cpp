#include "pch.h"
#include "CollisionDetector.h"
#include <string>

OcTree* ClientBoxTree = nullptr;
int32 OcTree::_maxLevel = 3;



float DotProduct(XMFLOAT3 a, XMFLOAT3 b) 
{
	return  a.x * b.x + a.y + b.y + a.z * b.z;
}

void OcTree::ReadBoundingBoxInfoFromFile(const char* filename)
{
	int i = 0;
	std::ifstream infile(filename);
	std::string line;
	while (getline(infile, line)) {
		// find the CENTER and EXTENTS substrings
		size_t centerDelimeter = line.find("Center:");
		size_t extentsDelimeter = line.find("Extents:");
		// extract the CENTER values
		float cx, cy, cz;
		if (centerDelimeter != std::string::npos)
		{
			size_t centerStart = line.find("(", centerDelimeter);
			size_t centerXend = line.find(",", centerStart);
			size_t centerYend = line.find(",", centerXend + 1);
			size_t centerZend = line.find(")", centerYend);

			if (centerStart != std::string::npos && centerXend != std::string::npos && centerYend != std::string::npos && centerZend != std::string::npos)
			{
				cx = atof(line.substr(centerStart + 1, centerXend - centerStart - 1).c_str());
				cy = atof(line.substr(centerXend + 1, centerYend - centerXend - 1).c_str());
				cz = atof(line.substr(centerYend + 1, centerZend - centerYend - 1).c_str());
				
			}
		}

		// extract the EXTENTS values
		float ex, ey, ez;
		if (extentsDelimeter != std::string::npos) 
		{
			size_t extentsStart = line.find("(", extentsDelimeter);
			size_t extentsXend = line.find(",", extentsStart);
			size_t extentsYend = line.find(",", extentsXend + 1);
			size_t extentsZend = line.find(")", extentsYend);

			if (extentsStart != std::string::npos && extentsXend != std::string::npos && extentsYend != std::string::npos && extentsZend != std::string::npos) {
				ex = atof(line.substr(extentsStart + 1, extentsXend - extentsStart - 1).c_str());
				ey = atof(line.substr(extentsXend + 1, extentsYend - extentsXend - 1).c_str());
				ez = atof(line.substr(extentsYend + 1, extentsZend - extentsYend - 1).c_str());
			}
		}
		BoundingBox bv;
		bv.Center = XMFLOAT3(cx, cy, cz);
		bv.Extents = XMFLOAT3(ex, ey, ez);
		//std::cout << "(" << cx << " " << cy << " " << cz << ") (" << ex << " " << ey << " " << ez << ")		[" << i++ << "]\n";
		AddBoundingBox(bv);
	}
	std::cout << "End BoundingBox Loaded\n";
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

bool OcTree::CheckCollision(DirectX::BoundingSphere& playerBox, XMFLOAT3& playerPos)
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

					// To Do Collision Respons
					XMFLOAT3 centerVec{ playerBox.Center.x - i.Center.x, 0.f , playerBox.Center.z - i.Center.z }; // 상자 중심으로 플레이어 상대적인 위치 벡터.

					//// 지형의 사각형 영역 중 구의 중심과 가장 가까운 정점의 x , z 좌표를 구한다.
					float closeX = clamp(centerVec.x, -1 * i.Extents.x, i.Extents.x);
					float closeZ = clamp(centerVec.z, -1 * i.Extents.z, i.Extents.z);

					//// 플레이어 좌표기준으로 얼만큼 거리인지 구한다.
					XMFLOAT3 closeDist{ 0,0,0 };
					if (centerVec.x >= 0) closeDist.x = centerVec.x - closeX; // 좌측에 있을 경우
					else  closeDist.x = closeX - centerVec.x; // 우측에 있을 경우


					if (centerVec.z >= 0) closeDist.z = centerVec.z - closeZ;
					else  closeDist.z = closeZ - centerVec.z;
					
					if (::fabs(playerBox.Radius - fabs(closeDist.x)) < ::fabs(playerBox.Radius - fabs(closeDist.z))) // offset 수치가 작은 쪽으로 계산.
					{
						if(centerVec.x < 0) playerBox.Center.x -= ::fabs((playerBox.Radius - closeDist.x)) * 1.2f;
						else playerBox.Center.x += ::fabs((playerBox.Radius - closeDist.x)) * 1.2f;
					}
					else
					{
						if(centerVec.z < 0) playerBox.Center.z -= ::fabs((playerBox.Radius - closeDist.z)) * 1.2f;
						else  playerBox.Center.z += ::fabs((playerBox.Radius - closeDist.z)) * 1.2f;
					}

					playerPos = playerBox.Center;
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
					rVal |= i->CheckCollision(playerBox,playerPos);
				}
				rVal |= i->CheckCollision(playerBox, playerPos);
			}
		}
		return rVal;
	}
}


bool OcTree::CheckRayCollision(Ray& ray)
{
	return false;
}