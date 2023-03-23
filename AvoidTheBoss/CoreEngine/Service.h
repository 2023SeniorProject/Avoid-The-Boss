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
//  � ���񽺿� ���õ� ������ �����ؼ� ��뤾��
// Server/Client Service
// ------------
class Service : public enable_shared_from_this<Service>
{
public:
	// 1. ���� Ÿ�� 2. iocpCore 3. ���� ���� �Լ�, 4. �ִ� ���� ����
	Service(ServiceType type, IocpCoreRef core, SessionFactory sf, int32 maxSessionCount = 1000);
	virtual ~Service();
	virtual bool Start() abstract; // �߻� Ű���� �����ϱ�
	bool CanStart() { return _sessionFactory != nullptr; }
	void SetSessionFactory(SessionFactory sf) { _sessionFactory = sf; }
	
	virtual void CloseService();

	SessionRef CreateSession(); // ������ �����԰� ���ÿ� iocp�ھ ����ϴ� ������ ����
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