#pragma once
// Worker Thread 기능을 클래스로 랩핑
//#include "RoomManager.h"
// IOCP에 등록할 수 있는 모든 오브젝트에 관해서 정의


class IocpObject;
class IocpCore
{

public:
	IocpCore();
	virtual ~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(class IocpObject* iocpObj); // socket과 session을 등록하는 함수
	bool Processing(uint32_t limit_time = INFINITE); // 실질적으로 일하는 worker_thread들이 iocp에서 완료된 업무를 탐색하는 곳
	virtual void Disconnect(int32 sid);
protected:
	HANDLE _hIocp = INVALID_HANDLE_VALUE;
};

