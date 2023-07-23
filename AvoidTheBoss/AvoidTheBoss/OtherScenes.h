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
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool);
	void		 BuildDefaultLightsAndMaterials();

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
public:
	CTitleScene() {}
	~CTitleScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool);
	virtual void MouseAction(const POINT& mp) override;
	void		 BuildDefaultLightsAndMaterials();
};

class CRoomScene : public CScene
{
	struct Member
	{
		int32 m_sid = -1;
		bool isReady;
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
	virtual void Update(HWND hWnd);
	virtual void UpdateReady(int32 sid, bool val) 
	{ 
		for (auto& i : m_members)
		{
			if(sid == i.m_sid) i.isReady = val;
		}
	}
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool);
	void		 BuildDefaultLightsAndMaterials();
	virtual void MouseAction(const POINT& mp) override;
};