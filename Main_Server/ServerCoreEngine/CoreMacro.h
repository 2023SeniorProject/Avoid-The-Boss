#pragma once

//=================
// CRASH
// �������� ũ������ ����Ű�� ����� ��ũ��
// ===================
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
											
		