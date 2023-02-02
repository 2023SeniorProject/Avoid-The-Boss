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
//        IOCP EVENT: � ������ iocp�� �ϰ��� ��ϵǾ��°�
// =========================
// ����� ������ ������ ù��° offset�� wsaoverlapped �� �ȴ�.
// �츮�� �����ð��� overexten�� ���� ��, ù ��� ������ wsaoverlapped�� �����Ͱ� ����.
class IocpEvent : public WSAOVERLAPPED
{
public:
	IocpEvent(EventType type);
	EventType	GetType() { return _comp; }
	void Init();
public:
	EventType _comp;
	IocpObjRef _ev_owner; // �ش� �̺�Ʈ ȣ�� IOCP��ü
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
	SessionRef _session = nullptr;
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
	SendEvent() : IocpEvent(EventType::Send) { };
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { };
};

