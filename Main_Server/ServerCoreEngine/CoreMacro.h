#pragma once

//=================
// CRASH
// 인위적인 크러쉬를 일으키게 만드는 매크로
// ===================
#define MOUTPUT
#define MINPUT

//#define USE_MANY_LOCKS(count)	RWLock _locks[count];
//#define USE_LOCK				USE_MANY_LOCKS(1)
//#define	READ_LOCK_IDX(idx)		rlock_guard readLockGuard_##idx(_locks[idx]);
//#define READ_LOCK				READ_LOCK_IDX(0)
//#define	WRITE_LOCK_IDX(idx)		wlock_guard writeLockGuard_##idx(_locks[idx]);
//#define WRITE_LOCK				WRITE_LOCK_IDX(0)

#define RWLOCK std::shared_mutex _lock;
#define RWLOCKS(count) std::shared_mutex _lock[count];
#define READ_IOCP_LOCK std::shared_lock<std::shared_mutex> rl(ServerIocpCore._lock);
#define WRITE_IOCP_LOCK std::unique_lock<std::shared_mutex> wr(ServerIocpCore._lock);
#define RLock  std::shared_lock<std::shared_mutex> rl(_lock);
#define WLock  std::unique_lock<std::shared_mutex> wr(_lock);


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
										