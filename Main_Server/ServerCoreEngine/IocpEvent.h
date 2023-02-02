#pragma once


class GameSession;


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
public:
	GameSession* _session = nullptr;
	char _buf[BUFSIZE / 2];
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
	SendEvent(char* packet) : IocpEvent(EventType::Send) 
	{
		_sWsaBuf.buf = _sbuf;
		_sWsaBuf.len = packet[0];
		memcpy(_sbuf, packet, sizeof(*packet));
	};
public:
	int32 _sid = -1;
	int32 _cid = -1;
	WSABUF _sWsaBuf;
	char _sbuf[BUFSIZE];
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) 
	{	 
		_rWsaBuf.buf = _rbuf;
		_rWsaBuf.len = BUFSIZE;
	};
public:
	int32 _sid = -1;
	int32 _cid = -1;
	WSABUF _rWsaBuf;
	char _rbuf[BUFSIZE];
};

