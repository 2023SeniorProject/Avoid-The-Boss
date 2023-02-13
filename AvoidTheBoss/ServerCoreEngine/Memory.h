#pragma once
#include "Allocator.h"


// Ư�� Ŭ������ ���ؼ� �޸� �Ҵ� + ������ ���ڸ� �Ѱ��ֵ������� // ���� ���ڸ� ���
template <class T , class... Args>
T* xnew(Args&&... args) // ���� ���� ���߿� ���� �����غ���
{
	T* memory = static_cast<T*>(Xalloc(sizeof(T))); // �޸� ������ �Ҵ����ֱ� ������ �����ڸ� ���� ȣ���ؾ��Ѵ�.
	// placement new // �̹� �ִ� �޸𸮿��ٰ� ������ ȣ����� ���� �غ���.
	new(memory)T(forward<Args>(args)...);
	return memory;
}


template<class T>
void xdelete(T* ptr)
{
	ptr->~T(); // ���� ���� �Ҹ��ڸ� ȣ���� �ش�.
	Xrelease(ptr);
}
