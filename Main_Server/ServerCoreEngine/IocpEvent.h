#pragma once


class Session;


//==========================
//        IOCP EVENT TYPE
// =========================
enum  class EventType : int8
{
	Accept,
	Connect,
	Disconnect,
	Send,
	Recv
};


//==========================
//        IOCP EVENT: 어떤 사유로 iocp에 일감이 등록되었는가
// =========================
// 상속을 받으면 무조건 첫번째 offset이 wsaoverlapped 가 된다.
// 우리가 수업시간에 overexten을 만들 때, 첫 멤버 변수로 wsaoverlapped를 넣은것과 같다.
class IocpEvent : public WSAOVERLAPPED
{
public:
	IocpEvent(EventType type);
	EventType	GetType() { return _comp; }
	void Init();
public:
	EventType _comp;
};


//AcceptEx 함수 인자와 관련된 정보들을 추가로 가지고 있어야한다.
// 주의 ! virtual 소멸자를 사용하지 않는다. 
// offset 메모리가 다른 값으로 채워질 수 있기 때문이다.

// ==== 서버 연결 관련 이벤트 ====
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { };
	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }
private:
	Session* _session = nullptr;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { };
};


// ==== 송 수신 관련 이벤트 ====
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { };
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { };
};

