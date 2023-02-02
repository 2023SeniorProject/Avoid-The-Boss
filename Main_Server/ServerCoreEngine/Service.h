#pragma once
#include "IocpCore.h"
#include "AcceptManager.h"
#include <functional>

enum class ServiceType
{
	Server,
	Client
};
using SessionFactory = function<SessionRef(void)>; 


// ------------
//  어떤 서비스와 관련된 것인지 구별해서 사용ㅎ자
// Server/Client Service
// ------------
class Service : public enable_shared_from_this<Service>
{
public:
	// 1. 서비스 타입 2. iocpCore 3. 세션 생성 함수, 4. 최대 동접 제한
	Service(ServiceType type, IocpCoreRef core, SessionFactory sf, int32 maxSessionCount = 1000);
	virtual ~Service();
	virtual bool Start() abstract; // 추상 키워드 공부하기
	bool CanStart() { return _sessionFactory != nullptr; }
	void SetSessionFactory(SessionFactory sf) { _sessionFactory = sf; }
	
	virtual void CloseService();

	SessionRef CreateSession(); // 세션을 생성함과 동시에 iocp코어에 등록하는 역할을 수행
	void AddSession(SessionRef session);
	void ReleaseSession(int32 sessionIdx);

public:
	USE_LOCK;
	ServiceType _type;
	IocpCoreRef _core;
	int32 _sessionCnt = 0;
	int32 _maxSessionCnt;
	SessionFactory _sessionFactory;
	unordered_map<int32, SessionRef> _clients;
};

class ClientService : public Service
{
public:
	ClientService(IocpCoreRef core, SessionFactory sf, int32 maxSessionCount = 1000);
	virtual ~ClientService() {};
	virtual bool Start() override;

};

class ServerService : public Service
{
public:
	ServerService(IocpCoreRef core, SessionFactory sf, int32 maxSessionCount = 1000);
	virtual ~ServerService() {};
	virtual bool Start() override;
	virtual void CloseService();
private:
	AcceptMgrRef _acceptMgr;
};