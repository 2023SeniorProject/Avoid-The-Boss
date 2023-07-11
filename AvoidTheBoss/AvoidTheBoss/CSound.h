#pragma once

#include "FMOD\inc\fmod.hpp" 

using namespace FMOD;

class CSound {
	FMOD_SYSTEM* pSystem;
	FMOD_SOUND* pSound[20];
	FMOD_CHANNEL* pChannel[5];
public:
	CSound();
	~CSound();
	void SoundSystem();
	void MyPlaySound(int nSound, int nChannel);
	void SoundStop(int nChannel);
	void SoundPause(int nChannel);
	void SoundResume(int nChannel);
	void SoundRelease();
	void SetVolum(int nChannel, float volume);
};