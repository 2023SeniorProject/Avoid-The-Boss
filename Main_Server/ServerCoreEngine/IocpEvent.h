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
//        IOCP EVENT: � ������ iocp�� �ϰ��� ��ϵǾ��°�
// =========================
// ����� ������ ������ ù��° offset�� wsaoverlapped �� �ȴ�.
// �츮�� �����ð��� overexten�� ���� ��, ù ��� ������ wsaoverlapped�� �����Ͱ� ����.
class IocpEvent : public WSAOVERLAPPED
{
public:
	IocpEvent(EventType type);
	void Init();
public:
	EventType _comp;
};


//AcceptEx �Լ� ���ڿ� ���õ� �������� �߰��� ������ �־���Ѵ�.
// ���� ! virtual �Ҹ��ڸ� ������� �ʴ´�. 
// offset �޸𸮰� �ٸ� ������ ä���� �� �ֱ� �����̴�.

// ==== ���� ���� ���� �̺�Ʈ ====
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


// ==== �� ���� ���� �̺�Ʈ ====
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

