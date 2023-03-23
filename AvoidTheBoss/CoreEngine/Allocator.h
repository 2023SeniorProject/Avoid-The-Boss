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
// �޸� ���� ���׸� ã�µ� ���� ������ ����̶�� �� �� �ִ�.
// ������ �޸𸮿� �����Ͽ� ���� ��� ��Ȳ�� �߻��ϴ��� �� ��� �ſ� ���� ��Ȳ
// 
// 
// ============

class StompAllocator
{
	enum  { PAGESIZE = 0x1000 }; // page �������� ����� �ش��ϴ� ���� �Է��� �� �ִ�.
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

	T* allocate(size_t count) // ���𰡸� �Ҵ�
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(Xalloc(size));
		// ��ü Ÿ�� * ī��Ʈ ��ŭ�� �޸� ������
	}
	void deallocate(T* ptr, size_t count)
	{
		Xrelease(ptr);
	}
};