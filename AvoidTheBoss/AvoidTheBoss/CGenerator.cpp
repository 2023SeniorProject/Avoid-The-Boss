#include "pch.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 1.25f;
}

CGenerator::CGenerator(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks, int number)
{
	CLoadedModelInfo* pGeneratorModel = pModel;

	if (!pGeneratorModel)
		pGeneratorModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Generator.bin", NULL, Layout::GENERATOR);

	SetChild(pGeneratorModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pGeneratorModel);
}
CGenerator::~CGenerator()
{
}

void CGenerator::Update(float fTimeElapsed)
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
			}
			else
			{
				m_pSkinnedAnimationController->SetTrackEnable(0, false);
				m_bSwitchAnimationOn = false;
				m_nAnimationCount = 0;
			}
		}
	}
}

void CGenerator::Animate(float fTimeElapsed)
{	
	CGameObject::Animate(fTimeElapsed);
}