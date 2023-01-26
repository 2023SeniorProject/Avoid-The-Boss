#pragma once

//=================
// CRASH
// 인위적인 크러쉬를 일으키게 만드는 매크로
// ===================
#define MOUTPUT
#define MINPUT

#define USE_MANY_LOCKS(count)	RWLock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)		rlock_guard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK				READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)		wlock_guard writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

#define CRASH(cause) 						 \
{											 \
uint32* crash = nullptr; 					 \
__analysis_assume(crash != nullptr); 		 \
* crash = 0xDEADBEEF;						 \
}											 \

#define ASSERT_CRAHS(expr)					\
{											\
	if(!(expr))								\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
											\
}											\
											
		