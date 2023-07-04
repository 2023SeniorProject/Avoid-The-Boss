#pragma once
//#include "Shader.h"
//#include "Player.h"
//#include "CEmployee.h"
//#include "CBoss.h"
//#include "CGenerator.h" // ����ġ �и�

#include "CGameScene.h"
#include "ClientPacketEvent.h"


class Scheduler;
class CGenerator;


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
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	

	virtual void AnimateObjects();
	void		 ChangeMyPlayerCamera();
	
public : // SceneInterface ��� �Լ�
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
public: // ���´� �ۼ� �Լ�
	CPlayer* GetScenePlayerBySid(const int32 sid);
	CPlayer* GetScenePlayerByIdx(const int32 idx);
	CGenerator* GetSceneGenByIdx(const int32 idx);

	void InitGame(void* packet ,int32 sid);
	void StopTimer() { m_timer.Stop(); }
	void StartTimer() { m_timer.Start(); }
	void AddEvent(queueEvent*, float);
	void Exit();
public:
	
	CCamera*							m_pCamera;
	WCHAR								txtFrameBuf[20];

	
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT								m_ptOldCursorPos;



	//int									m_nGameObjects = 0;
	//CGameObject**						m_ppGameObjects = NULL;

	//int									m_nHierarchicalGameObjects = 0;
	//CGameObject**						m_ppHierarchicalGameObjects = NULL;

	//int									m_nShaders = 0;
	//CShader**							m_ppShaders = NULL;
	//CSkyBox*							m_pSkyBox = NULL;

	//LIGHT*								m_pLights = NULL;
	//int									m_nLights = 0;
	//XMFLOAT4							m_xmf4GlobalAmbient;
	//ID3D12Resource*						m_pd3dcbLights = NULL;
	//LIGHTS*								m_pcbMappedLights = NULL;


private:
	Timer m_timer;
// ========== ���� ó���� ���� ����ϴ� ������ ==============
public: // ���� �ִ� ������Ʈ ���� ����
	CPlayer*					m_players[4];
	int16						m_playerIdx = 0;
	int16						m_lastKeyInput = 0;
	// ������
	int							m_nGenerator = 3;
	CGenerator**				m_ppGenerator = NULL;
public:
	int32						m_ActiveGeneratorCnt = 0; // Ȱ��ȭ �� ����ġ ī��Ʈ;
public:
	bool						m_bEmpExit = false;
	bool						m_bBossWin = false;
public:
	Scheduler* m_jobQueue;
	std::shared_mutex m_jobQueueLock;
public:

	int32				m_curFrame;
	int32				m_cid = -1;
	int32				m_sid = -1;
};

