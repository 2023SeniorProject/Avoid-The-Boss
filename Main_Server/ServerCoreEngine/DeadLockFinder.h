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
	map<int32, set<int32>> _lHistory; // 각 노드별 방문 기록

	Mutex _lock;
private: // 사이클 체크하기 위한 값들
	vector<int32> _discoveredOrder; // 노드가 발견된 순서를 기록하는 배열
	int32 _discoveredCnt; // 발견된 노드 카운팅
	vector<int32> _parent; // 나를 발견한 부모 기록
	vector<bool> _finished; // DFS(i) 종료 여부 파악
};

