#pragma once

//============
// BasicAllocator
//============
class BasicAllocator
{
public:

	static void* Alloc(size_t s); 
	static void  Rel(void* ptr);
};


// ============
// Stomp Allocator 
// 메모리 오염 버그를 찾는데 가장 유용한 기능이라고 볼 수 있다.
// 오염된 메모리에 관련하여 언제 어느 상황에 발생하는지 알 기란 매우 힘든 상황
// 
// 
// ============

class StompAllocator
{
	enum  { PAGESIZE = 0x1000 }; // page 사이즈의 배수에 해당하는 값을 입력할 수 있다.
public:

	static void* Alloc(int32 s);
	static void  Rel(void* ptr);

};


//============
// STL Allocator
//============

template<class T>
class STLAllocator
{
public:
	using value_type = T;

	STLAllocator() {};
	
	template<class Other>
	STLAllocator(const STLAllocator<Other>&) {};

	T* allocate(size_t count) // 무언가를 할당
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(Xalloc(size));
		// 객체 타입 * 카운트 만큼이 메모리 사이즈
	}
	void deallocate(T* ptr, size_t count)
	{
		Xrelease(ptr);
	}
};