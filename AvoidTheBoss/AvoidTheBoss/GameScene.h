#pragma once
//#include "Shader.h"
//#include "Player.h"
//#include "CEmployee.h"
//#include "CBoss.h"
//#include "CGenerator.h" // 스위치 분리

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
	//씬에서 마우스와 키보드 메시지를 처리한다.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	
	virtual void AnimateObjects();	
public : // SceneInterface 상속 함수
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND& hWnd);
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool Raster);
public: // 오승담 작성 함수
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
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT								m_ptOldCursorPos;
private:
	Timer								m_timer;
// ========== 서버 처리를 위해 사용하는 변수들 ==============
public: 
	// 씬에 있는 오브젝트 관련 변수
	CPlayer*					m_players[4];
	int16						m_playerIdx = 0;
	int16						m_lastKeyInput = 0;

	// 발전기
	int							m_nGenerator = 3;
	CGenerator**				m_ppGenerator = NULL;

public:
	int32						m_ActiveGeneratorCnt = 0; // 활성화 된 스위치 카운트;
public:
	Atomic<int32>				m_remainPlayerCnt = PLAYERNUM; // 남아 있는 플레이어
	Atomic<int32>				m_ExitedPlayerCnt = 0;
	bool						m_bEmpExit = false;

public:
	Scheduler*					m_jobQueue;
	std::shared_mutex			m_jobQueueLock;
public:
	XMFLOAT3					m_xmf3ClearPoint[3]; // 클리어 좌표
public:
	int32						m_curFrame;
public:
	bool						m_exitSoundOn = false;
};

