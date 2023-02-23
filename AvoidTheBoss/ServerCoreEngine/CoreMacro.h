#pragma once

//=================
// CRASH
// 인위적인 크러쉬를 일으키게 만드는 매크로
// ===================
#define MOUTPUT
#define MINPUT


#define RWLOCK std::shared_mutex _lock;
#define RWLOCKS(count) std::shared_mutex _lock[count];
#define READ_SERVER_LOCK std::shared_lock<std::shared_mutex> rl(ServerIocpCore._lock);
#define WRITE_SERVER_LOCK std::unique_lock<std::shared_mutex> wr(ServerIocpCore._lock);
#define RLock  std::shared_lock<std::shared_mutex> rl(_lock);
#define WLock  std::unique_lock<std::shared_mutex> wr(_lock);

// room  클래스에서 리스트 관리할 때 사용하는 락
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
										