#pragma once
#include "GameFramework.h"
#include "Session.h"
#include "Player.h"
#include "DummyPlayer.h"

// =============== new Client Session ================
// ===================================================
// ===================================================

class CSession : public IocpObject
{
	
public:
	CSession();
	virtual ~CSession();
public:
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int16  _loginOk = -3;
public:
	SOCKET _sock = INVALID_SOCKET;
	CGameFramework _mainGame;
	RecvEvent _rev;
	RWLOCK;
};

// ========= new Iocp Core ==============

class ClientMananger : public IocpCore
{
	friend class CSession;
public:
	ClientMananger();
	~ClientMananger();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	//void DestroyGame() { _client->_mainGame.OnDestroy(); }
	//void InitGameLoop(HINSTANCE hInst, HWND hWnd) { _client->_mainGame.OnCreate(hInst, hWnd); }
	//void GameLoop() { _client->_mainGame.FrameAdvance(); }
	void InputProcessing(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { _client->_mainGame.OnProcessingWindowMessage(hWnd, message, wParam, lParam); }
	virtual void Disconnect(int32 sid) override;
public:
	CSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class ClientMananger clientCore;