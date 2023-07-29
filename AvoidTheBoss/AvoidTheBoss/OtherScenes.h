#pragma once
#include "CScene.h"

const int32 MAX_ROOM = 100;

class CLobbyScene : public CScene
{
	struct Room
	{
		int32 member = 0;
		int32 idx = 0;
		ROOM_STATUS status = ROOM_STATUS::EMPTY;
	};
public:
	int32	 m_curPage;
	int32	 m_lastPage;
	Room	 m_rooms[MAX_ROOM];
	int32	 m_selected_rm = -1;
	CPlayer* m_player = NULL;
public:
	CLobbyScene() {}
	~CLobbyScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND& hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool);
	void		 BuildDefaultLightsAndMaterials();
	void ReleaseObjects() {}
	virtual void MouseAction(const POINT& mp) override;

	void ChangePage(int32);
	void UpdateRoomText(int32, int32);
	void UpdateRoomStatus(int32 rn, int32 mem) 
	{ 
		m_rooms[rn].member = mem; 
		for (int i = 0; i < 5; ++i)UpdateRoomText(i, -1);
	};
	Room& GetRoom(int32 rm) { return m_rooms[rm]; }
	int32 GetCurPage() { return m_curPage; };
};

class CTitleScene : public CScene
{
	int32 focus = 0;
	bool cap = false;
	CPlayer* m_player = NULL;
	Timer m_timer;
public:
	CTitleScene() {}
	~CTitleScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND& hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool);
	virtual void MouseAction(const POINT& mp) override;
	void		 BuildDefaultLightsAndMaterials();
};

class CRoomScene : public CScene
{
	struct Member
	{
		int32 m_sid = -1;
		bool isReady = false;
	};
public:
	Member m_members[4];
	std::mutex m_memLock;
	int32 m_rmnum;
public:
	CRoomScene() 
	{
		for (auto& i : m_members) i.isReady = false;
	}
	~CRoomScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND& hWnd);
	virtual void UpdateReady(int32 sid, bool val) 
	{ 
		for (auto& i : m_members)
		{
			if(sid == i.m_sid) i.isReady = val;
		}
	}
	void ReleaseObjects() {}
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool);
	void		 BuildDefaultLightsAndMaterials();
	virtual void MouseAction(const POINT& mp) override;
};

class CResultScene : public CScene
{
public:
	int32 m_pidx = -1;
	int32 m_case = 0; // 1  escape 2 arrested
	
	
	// 사장
	// 탈출 직원 수
	int32 m_exitPlayerCnt = 0;
	// 죽인 횟수

	// 직원
	int32 m_deadCnt;   //  죽은 횟수
	int32 m_activeCnt; //  발전기 활성화 횟수

	Timer m_timer;
	float m_showTime = 4.0f; // 결과창 보여주는 시각
	
public:
	CResultScene() {}
	~CResultScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList) {};
	virtual void ProcessInput(HWND& hWnd) {};
	virtual void Update(HWND& hWnd) 
	{
		m_timer.Tick(0.0f);
		if (m_showTime > 0) m_showTime -= m_timer.GetTimeElapsed();
		if (m_showTime < 0) mainGame.ChangeScene(CGameFramework::SCENESTATE::LOBBY);
	};
	void ReleaseObjects() {}
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster) {};
	
	virtual void MouseAction(const POINT& mp) override {};
};