#pragma once

#include "FMOD\inc\fmod.hpp" 

using namespace FMOD;

class CSound {
	FMOD_SYSTEM* pSystem;
	FMOD_SOUND* pSound[22];
	FMOD_CHANNEL* pChannel[22];
	// 0 : �������
	// 1 : ������� temp
	// 2 : UI ��ư
	// 3 : �Ѿ�, �ǰ�
	// 4 : �÷��̾� �̵�
	// 5 : Ż�� ������Ʈ (��)
	// 6 : ������
	// 7 : ���̷�

public:
	CSound();
	~CSound();
	void SoundSystem();
	void PlayBackGroundSound(int nSound, int nChannel);
	void PlaySound(int nSound, int nChannel);
	void SoundStop(int nChannel);
	void SoundPause(int nChannel);
	void SoundResume(int nChannel);
	void SoundRelease();
	void SetVolum(int nChannel, float volume);
};