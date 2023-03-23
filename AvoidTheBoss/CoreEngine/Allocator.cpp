#include "pch.h"
#include "Allocator.h"

void* BasicAllocator::Alloc(size_t s)
{
	return ::malloc(s); // �ش� �����ŭ �޸� �����Ҵ�
}

void BasicAllocator::Rel(void* ptr)
{
	::free(ptr); // �����Ҵ��� �޸� ��ȯ
}

void* StompAllocator::Alloc(int32 s)
{
	//���� �Ҵ��ش޶�� �� ������� ���� ����� ������ ������ ��ŭ�� �Ҵ�
	// 4kb -> 4096�� ��� 
	const int64 pageCount = (s + PAGESIZE - 1) / PAGESIZE;
	// ���� s = 4 
	// 4 + 4095 = 4099 
	// 4099 / 4095 = 1.xx
	// PAGESIZE -1 : �츮�� ���� ������ �������� ����� ���� ������ ��ġ�� ���Դٰ� �� ���
	// like 4096  4095 + 4096 / 4096 = 1.99xxx --> 1����� ����
	const int64 dataOffset = pageCount * PAGESIZE - s;
	
	void* baseAddress =  ::VirtualAlloc(NULL, pageCount * PAGESIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
	// �޸� ���� �÷ο� ������ ���� ���Ѵ�..
	// 4kb������ �������ϱ� ������ �ƹ��� �������� �����ٰ� �ϴ��� ����ū �޸� ������ �Ҵ��ϰ� �Ǵ� ���̴�.
	// �׷��� 4kb ���� �ȿ��� ���� �÷ο� ������ ���� ���� ����.
	// �ذ�� --> 4kb�ȿ��� �Ҵ��� ��, �� �κп��ٰ� ��ġ�ϴ� ���̴�! ����÷ο� ������ �߻��� ���� ������
	// ��κ��� ������ ���� �÷ο쿡�� �߻��ϴ�, ���� �÷ο쿡�� ����ִ� ���� �ո����̶�� �� �� �ִ�.
}

void StompAllocator::Rel(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGESIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
