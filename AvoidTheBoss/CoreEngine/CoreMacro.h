#pragma once

//=================
// CRASH
// �������� ũ������ ����Ű�� ����� ��ũ��
// ===================
#define MOUTPUT
#define MINPUT


#define RWLOCK std::shared_mutex _lock;
#define RWLOCKS(count) std::shared_mutex _lock[count];
#define READ_SERVER_LOCK std::shared_lock<std::shared_mutex> rl(ServerIocpCore._lock);
#define WRITE_SERVER_LOCK std::unique_lock<std::shared_mutex> wr(ServerIocpCore._lock);
#define RLock  std::shared_lock<std::shared_mutex> rl(_lock);
#define WLock  std::unique_lock<std::shared_mutex> wr(_lock);

// room  Ŭ�������� ����Ʈ ������ �� ����ϴ� ��
#define RMListLock std::shared_mutex _listLock;
#define RListLock std::shared_lock<std::shared_mutex> lrl(_listLock);
#define WListLock std::unique_lock<std::shared_mutex> lwl(_listLock);

#ifdef _DEBUG
#define Xalloc(size)					StompAllocator::Alloc(size)
#define Xrelease(ptr)					StompAllocator::Rel(ptr)
#else
#define xallocate(size)					BasicAllocator::Alloc(size)
#define xrelease(ptr)					BasicAllocator::Rel(ptr)
#endif

#define CRASH(cause) 						 \
{											 \
uint32* crash = nullptr; 					 \
__analysis_assume(crash != nullptr); 		 \
* crash = 0xDEADBEEF;						 \
}											 \

#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
											\
}											\
										