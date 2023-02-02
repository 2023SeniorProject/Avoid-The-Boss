#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "AcceptManager.h"

Service::Service(ServiceType type, IocpCoreRef core, SessionFactory sf, int32 maxSessionCount)
	: _type(type), _sessionFactory(sf), _core(core),_maxSessionCnt(maxSessionCount)
{
}

Service::~Service()
{
}

void Service::CloseService()
{
}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory(); // 세션을 생성한다.
	if (_core->Register(session) == false) return nullptr; // iocp핸들에 등록하는데 실패하면 널 반환
	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_clients.try_emplace(_sessionCnt++, session); //  세션 추가

}

void Service::ReleaseSession(int32 sessionIdx)
{
	WRITE_LOCK;
	ASSERT_CRASH(_clients.erase(sessionIdx) != 0);
	_sessionCnt--;
}

ClientService::ClientService(IocpCoreRef core, SessionFactory sf, int32 maxSessionCount)
	:Service(ServiceType::Client,core,sf,maxSessionCount)
{
}

bool ClientService::Start()
{
	return true;
}

ServerService::ServerService(IocpCoreRef core, SessionFactory sf, int32 maxSessionCount)
	:Service(ServiceType::Server, core, sf, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false) return false;
	_acceptMgr = make_shared<AcceptManager>();
	if (_acceptMgr == nullptr) return false;
	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_acceptMgr->InitAccept(service) == false) return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}
