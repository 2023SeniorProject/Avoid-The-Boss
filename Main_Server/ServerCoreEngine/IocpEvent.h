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
};


//AcceptEx �Լ� ���ڿ� ���õ� �������� �߰��� ������ �־���Ѵ�.
// ���� ! virtual �Ҹ��ڸ� ������� �ʴ´�. 
// offset �޸𸮰� �ٸ� ������ ä���� �� �ֱ� �����̴�.

// ==== ���� ���� ���� �̺�Ʈ ====
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

