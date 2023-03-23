#include "pch.h"
#include "DeadLockFinder.h"

using namespace std;

void DeadLockFinder::PushLock(const char* name)
{
	lockG lg(_lock);
	int32 lockId = 0;
	auto findId = _findIdWithName.find(name);
	if (findId == _findIdWithName.end()) // �߰ߵ��� ���� Id = ó�� �߰ߵ� Id���? Ȥ�� ���� �����̶��?
	{
		lockId = (int32)(_findIdWithName.size());
		_findIdWithName[name] = lockId;
		_findNameWithId[lockId] = name;
		// ��� 
	}
	else
	{
		// �̹��� �ѹ� ã�� ���� �ִٸ�~ 
		//�ش��ϴ� lockId�� �����Ѵ�.
		lockId = findId->second;
	}

	// ����ִ� ���� �־��ٸ�?
	if (_ls.empty() == false) // ���ο� ���� �߰� ��,
	{
		// ������ �߰ߵ��� ���� ���̽���� ����Ŭ Ȯ��
		const int32 prevId = _ls.top();
		if (lockId != prevId)
		{
			set<int32>& tmpHistory = _lHistory[prevId];
			if (tmpHistory.find(lockId) == tmpHistory.end())
			{
				// ������ �湮 ����� �������� �߰ߵ��� ���� ������� ���� ��,
				tmpHistory.insert(lockId);
				IsCycle(); // ����Ŭ üũ
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
	_discoveredOrder = std::vector<int32>(lockCnt, -1); // ���� �ʱ�ȭ
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
		// �������� �ƴϰ� Dfs(there)�� ���� �ȳ����ٸ� there�� here�� parent��. --> ������
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
