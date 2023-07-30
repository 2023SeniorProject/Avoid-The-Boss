#include "pch.h"
#include "SoundManager.h"
#include "CSound.h"

SoundManager* SoundManager::instance = nullptr;
CSound* SoundManager::m_pSound = nullptr;

SoundManager::SoundManager()
{
	m_pSound = new CSound();
	m_pSound->SoundSystem();
}

SoundManager::~SoundManager()
{
	delete m_pSound;
}

void SoundManager::SoundSystem()
{
	m_pSound->SoundSystem();
}

void SoundManager::PlayBackGroundSound(int32 Scene)
{
	switch (Scene)
	{
	case 0:
	case 1:
	case 2:
		m_pSound->SoundStop(0);
		m_pSound->PlayBackGroundSound(0,0);
		break;
	case 3:
		m_pSound->SoundStop(0);
		m_pSound->PlayBackGroundSound(1, 0);
		break;
	}
}

void SoundManager::PlayObjectSound(int32 idx, int32 channel)
{
	m_pSound->PlaySoundA(idx, channel);
}

void SoundManager::SoundStop(int32 nChannel)
{
	m_pSound->SoundStop(nChannel);
}

void SoundManager::SoundPause(int32 nChannel)
{
	m_pSound->SoundPause(nChannel);
}

void SoundManager::SoundResume(int32 nChannel)
{
	m_pSound->SoundResume(nChannel);
}

void SoundManager::SoundRelease()
{
	m_pSound->SoundRelease();

}

void SoundManager::SetVolum(int nChannel, float volume)
{
	m_pSound->SetVolum(nChannel, volume);
}