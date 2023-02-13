#include "pch.h"
#include "Allocator.h"

void* BasicAllocator::Alloc(size_t s)
{
	return ::malloc(s); // 해당 사이즈만큼 메모리 동적할당
}

void BasicAllocator::Rel(void* ptr)
{
	::free(ptr); // 동적할당한 메모리 반환
}

void* StompAllocator::Alloc(int32 s)
{
	//내가 할당해달라고 한 사이즈와 가장 비슷한 페이지 사이즈 만큼을 할당
	// 4kb -> 4096의 배수 
	const int64 pageCount = (s + PAGESIZE - 1) / PAGESIZE;
	// 원리 s = 4 
	// 4 + 4095 = 4099 
	// 4099 / 4095 = 1.xx
	// PAGESIZE -1 : 우리가 만약 페이즈 사이즈의 배수와 거의 유사한 수치가 나왔다고 할 경우
	// like 4096  4095 + 4096 / 4096 = 1.99xxx --> 1사이즈가 나옴
	const int64 dataOffset = pageCount * PAGESIZE - s;
	
	void* baseAddress =  ::VirtualAlloc(NULL, pageCount * PAGESIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
	// 메모리 오버 플로우 현상은 잡지 못한다..
	// 4kb단위로 잡아줘야하기 때문에 아무리 작은값을 가진다고 하더라도 ㅈㄴ큰 메모리 영역을 할당하게 되는 것이다.
	// 그래서 4kb 단위 안에서 오버 플로우 현상을 잡을 수는 없다.
	// 해결법 --> 4kb안에서 할달할 때, 끝 부분에다가 배치하는 것이다! 언더플로우 문제가 발생할 수는 있지만
	// 대부분의 오류가 오버 플로우에서 발생하니, 오버 플로우에서 잡아주는 것이 합리적이라고 볼 수 있다.
}

void StompAllocator::Rel(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGESIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
