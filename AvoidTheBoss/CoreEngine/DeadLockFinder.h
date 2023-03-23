#pragma once
#include <stack>
#include <map>
#include <vector>
class DeadLockFinder
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void IsCycle();
private:
	void Dfs(int32 idx);
private:
	std::unordered_map<const char*, int32> _findIdWithName;
	std::unordered_map<int32, const char*> _findNameWithId;
	std::stack<int32> _ls;
	std::map<int32, std::set<int32>> _lHistory; // �� ��庰 �湮 ���

	Mutex _lock;
private: // ����Ŭ üũ�ϱ� ���� ����
	std::vector<int32> _discoveredOrder; // ��尡 �߰ߵ� ������ ����ϴ� �迭
	int32 _discoveredCnt; // �߰ߵ� ��� ī����
	std::vector<int32> _parent; // ���� �߰��� �θ� ���
	std::vector<bool> _finished; // DFS(i) ���� ���� �ľ�
};

