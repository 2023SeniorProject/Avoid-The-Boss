#pragma once
//#include "Shader.h"
//#include "Player.h"
//#include "CEmployee.h"
//#include "CBoss.h"
//#include "CGenerator.h" // ����ġ �и�

#include "CScene.h"
#include "ClientPacketEvent.h"


class Scheduler;
class CGenerator;
class CSound;

class CGameScene : public CScene
{
	friend class CSession;
	friend class queueEvent;
	friend class FrameEvent;
public:
	CGameScene();
	~CGameScene();
	virtual void InitScene() { m_timer.Reset(); }
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	
	virtual void AnimateObjects();	
public : // SceneInterface ��� �Լ�
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND& hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool Raster);
public: // ���´� �ۼ� �Լ�
	CPlayer* GetScenePlayerBySid(const int32 sid);
	CPlayer* GetScenePlayerByIdx(const int32 idx);
	CGenerator* GetSceneGenByIdx(const int32 idx);

	void InitGame(void* packet ,int32 sid);
	
	void StopTimer() { m_timer.Stop(); }
	void StartTimer() { m_timer.Start(); }
	void AddEvent(queueEvent*, float);

	void ExitReady();

	void ResetGame();
public:
	WCHAR								txtFrameBuf[20];
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT								m_ptOldCursorPos;
private:
	Timer								m_timer;
// ========== ���� ó���� ���� ����ϴ� ������ ==============
public: 
	// ���� �ִ� ������Ʈ ���� ����
	CPlayer*					m_players[4];
	int16						m_playerIdx = 0;
	int16						m_lastKeyInput = 0;

	// ������
	int							m_nGenerator = 3;
	CGenerator**				m_ppGenerator = NULL;

public:
	int32						m_ActiveGeneratorCnt = 0; // Ȱ��ȭ �� ����ġ ī��Ʈ;
public:
	Atomic<int32>				m_remainPlayerCnt = PLAYERNUM; // ���� �ִ� �÷��̾�
	Atomic<int32>				m_ExitedPlayerCnt = 0;
	bool						m_bEmpExit = false;

public:
	Scheduler*					m_jobQueue;
	std::shared_mutex			m_jobQueueLock;
public:
	XMFLOAT3					m_xmf3ClearPoint[3]; // Ŭ���� ��ǥ
public:
	int32						m_curFrame;
public:
	bool						m_exitSoundOn = false;
};

