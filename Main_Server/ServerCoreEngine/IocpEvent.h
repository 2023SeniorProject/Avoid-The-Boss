#pragma once

class Session;

enum  class EventType : int8
{
	Accept,
	Connect,
	Disconnect,
	Send,
	Recv
};

class IocpEvent : public WSAOVERLAPPED
{
public:
	IocpEvent(EventType et);
	EventType	GetType() { return _comp; }
	void Init();
public:
	EventType _comp;
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { };
	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }
private:
	Session* _session = nullptr;
};

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