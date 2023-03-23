#pragma once
#include "Allocator.h"


// 특정 클래스에 대해서 메모리 할당 + 생성자 인자를 넘겨주도록하자 // 가변 인자를 사용
template <class T , class... Args>
T* xnew(Args&&... args) // 보편 참조 나중에 따로 공부해보자
{
	T* memory = static_cast<T*>(Xalloc(sizeof(T))); // 메모리 영역만 할당해주기 때문에 생성자를 직접 호출해야한다.
	// placement new // 이미 있는 메모리에다가 생성자 호출까지 같이 해보자.
	new(memory)T(forward<Args>(args)...);
	return memory;
}


template<class T>
void xdelete(T* ptr)
{
	ptr->~T(); // 삭제 전에 소멸자를 호출해 준다.
	Xrelease(ptr);
}
