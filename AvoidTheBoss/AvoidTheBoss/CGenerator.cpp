#include "pch.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 1.25f;
}

CGenerator::CGenerator(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks, int number)
{
	radius = 1.25f;

	CLoadedModelInfo* pSirenModel = pModel;

	const char* path[3] = {
		"Model/Button1.bin",
		"Model/Button2.bin",
		"Model/Button3.bin"
	};
	if (!pSirenModel)
		pSirenModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, path[number], NULL, Layout::SIREN);

	SetChild(pSirenModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pSirenModel);
}
CGenerator::~CGenerator()
{
}

void CGenerator::Animate(float fTimeElapsed)
{
	if (m_pSkinnedAnimationController)
	{
		if (m_bSwitchAnimationOn)
		{
			if (m_nAnimationCount > 0)
			{
				m_pSkinnedAnimationController->SetTrackEnable(0, true);
				m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);

				m_nAnimationCount--;
				CGameObject::Animate(fTimeElapsed);
			}
			else
			{
				m_bSwitchAnimationOn = false;
				m_nAnimationCount = 0;
			}
		}
	}
}