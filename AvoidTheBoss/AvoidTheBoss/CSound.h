#pragma once

#include "FMOD\inc\fmod.hpp" 

using namespace FMOD;

class CSound {
	FMOD_SYSTEM* pSystem;
	FMOD_SOUND* pSound[22];
	FMOD_CHANNEL* pChannel[22];
	// 0 : 배경음악
	// 1 : 배경음악 temp
	// 2 : UI 버튼
	// 3 : 총알, 피격
	// 4 : 플레이어 이동
	// 5 : 탈출 오브젝트 (문)
	// 6 : 발전기
	// 7 : 사이렌

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