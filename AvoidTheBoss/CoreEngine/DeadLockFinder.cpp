#include "pch.h"
#include "DeadLockFinder.h"

using namespace std;

void DeadLockFinder::PushLock(const char* name)
{
	lockG lg(_lock);
	int32 lockId = 0;
	auto findId = _findIdWithName.find(name);
	if (findId == _findIdWithName.end()) // 발견되지 않은 Id = 처음 발견된 Id라면? 혹은 시작 지점이라면?
	{
		lockId = (int32)(_findIdWithName.size());
		_findIdWithName[name] = lockId;
		_findNameWithId[lockId] = name;
		// 등록 
	}
	else
	{
		// 이번에 한번 찾은 적이 있다면~ 
		//해당하는 lockId를 추출한다.
		lockId = findId->second;
	}

	// 잡고있는 락이 있었다면?
	if (_ls.empty() == false) // 새로운 간선 발견 시,
	{
		// 기존에 발견되지 않은 케이스라면 사이클 확인
		const int32 prevId = _ls.top();
		if (lockId != prevId)
		{
			set<int32>& tmpHistory = _lHistory[prevId];
			if (tmpHistory.find(lockId) == tmpHistory.end())
			{
				// 기존의 방문 기록을 뒤져봐서 발견되지 않은 정보라고 했을 때,
				tmpHistory.insert(lockId);
				IsCycle(); // 사이클 체크
			}
		}
	}
	_ls.push(lockId);
}

void DeadLockFinder::PopLock(const char* name)
{
	lockG lg(_lock);
	if (_ls.empty()) CRASH("TRY EMPTY STACK POP");
	int32 lockId = _findIdWithName[name];
	if (_ls.top() != lockId) CRASH("INVALID_ID_UNLOCK");

	_ls.pop();
}

void DeadLockFinder::IsCycle()
{
	const int32 lockCnt = (int32)(_findIdWithName.size());
	_discoveredOrder = std::vector<int32>(lockCnt, -1); // 벡터 초기화
	_discoveredCnt = 0;
	_finished = std::vector<bool>(lockCnt, false);
	_parent = std::vector<int32>(lockCnt, -1);

	for (int32 lockId = 0; lockId < lockCnt; lockId++) Dfs(lockId);
	_discoveredOrder.clear();
	_parent.clear();
	_finished.clear();
}

void DeadLockFinder::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;
	_discoveredOrder[here] = +_discoveredCnt++;

	auto findIt = _lHistory.find(here);
	if (findIt == _lHistory.end())
	{
		_finished[here] = true;
		return;
	}
	std::set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		// 순방향이 아니고 Dfs(there)가 아직 안끝났다면 there는 here의 parent다. --> 역방향
		if (_finished[here] == false)
		{
			int32 now = here;
			printf("%s --> %s \n", _findNameWithId[here], _findNameWithId[there]);
			while (true)
			{
				printf("%s --> %s \n", _findNameWithId[_parent[now]], _findNameWithId[now]);
				now = _parent[now];
				if (now == there) break;
			}
			CRASH("DEAD_LOCK");
		}
	}
	
	
	_finished[here] = true;
}
