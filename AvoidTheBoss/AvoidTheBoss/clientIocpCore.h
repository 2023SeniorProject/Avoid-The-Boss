#pragma once
#include "GameFramework.h"
#include "Session.h"
#include "Player.h"
#include "DummyPlayer.h"

// =============== new Client Session ================
// ===================================================
// ===================================================

class CClientSession : public IocpObject
{
	
public:
	CClientSession();
	virtual ~CClientSession();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
	void PrintPInfo()
	{
		_mainGame.m_pScene->_player->GetPosition().x;
	}
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int8 _curScene = -1;
public:
	SOCKET _sock = INVALID_SOCKET;

	std::mutex _playerLock;
	std::mutex _otherLock;
	CGameFramework _mainGame;
	RecvEvent _rev;
	RWLOCK;
};

// ========= new Iocp Core ==============

class CIocpCore : public IocpCore
{
	friend class CClientSession;
public:
	CIocpCore();
	~CIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	void DestroyGame() { _client->_mainGame.OnDestroy(); }
	void InitGameLoop(HINSTANCE hInst, HWND hWnd) { _client->_mainGame.OnCreate(hInst, hWnd); }
	void GameLoop() { _client->_mainGame.FrameAdvance(); }
	void InputProcessing(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { _client->_mainGame.OnProcessingWindowMessage(hWnd, message, wParam, lParam); }
	virtual void Disconnect(int32 sid) override;
public:
	CClientSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class CIocpCore clientIocpCore;