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
	unordered_map<const char*, int32> _findIdWithName;
	unordered_map<int32, const char*> _findNameWithId;
	stack<int32> _ls;
	map<int32, set<int32>> _lHistory; // �� ��庰 �湮 ���

	Mutex _lock;
private: // ����Ŭ üũ�ϱ� ���� ����
	vector<int32> _discoveredOrder; // ��尡 �߰ߵ� ������ ����ϴ� �迭
	int32 _discoveredCnt; // �߰ߵ� ��� ī����
	vector<int32> _parent; // ���� �߰��� �θ� ���
	vector<bool> _finished; // DFS(i) ���� ���� �ľ�
};

