#include "pch.h"
#include "CSound.h"
#pragma comment(lib,"FMOD/lib/fmod_vc.lib")

CSound::CSound()
{
}

CSound::~CSound()
{
}

void CSound::SoundSystem()
{
	FMOD_System_Create(&pSystem, FMOD_VERSION);

	FMOD_System_Init(pSystem, 20, FMOD_INIT_NORMAL, nullptr);

	// 배경음악
	FMOD_System_CreateSound(pSystem, "Sound/Danger_Background.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[0]); //0
	FMOD_System_CreateSound(pSystem, "Sound/Danger_Background2.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[1]); //1

	// 보스
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot1.mp3", FMOD_LOOP_OFF, nullptr, &pSound[2]); // 2
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot2.mp3", FMOD_LOOP_OFF, nullptr, &pSound[3]);
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot3.mp3", FMOD_LOOP_OFF, nullptr, &pSound[4]);
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot4.mp3", FMOD_LOOP_OFF, nullptr, &pSound[5]);

	// 공장직원
	FMOD_System_CreateSound(pSystem, "Sound/Button_Press.mp3", FMOD_LOOP_OFF, nullptr, &pSound[6]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Down.mp3", FMOD_LOOP_OFF, nullptr, &pSound[7]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Hit.mp3", FMOD_LOOP_OFF, nullptr, &pSound[8]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Hit2.mp3", FMOD_LOOP_OFF, nullptr, &pSound[9]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run1.mp3", FMOD_LOOP_OFF, nullptr, &pSound[10]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run2.mp3", FMOD_LOOP_OFF, nullptr, &pSound[11]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run3.mp3", FMOD_LOOP_OFF, nullptr, &pSound[12]);	
	FMOD_System_CreateSound(pSystem, "Sound/Character_Scream.mp3", FMOD_LOOP_OFF, nullptr, &pSound[13]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Walk.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[14]);

	// 오브젝트
	FMOD_System_CreateSound(pSystem, "Sound/Dangerous_Alarm.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[15]);
	FMOD_System_CreateSound(pSystem, "Sound/Emergency_Door_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[16]);
	FMOD_System_CreateSound(pSystem, "Sound/Generator.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[17]);
	FMOD_System_CreateSound(pSystem, "Sound/Hangar_Door_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[18]);
	FMOD_System_CreateSound(pSystem, "Sound/Shutter_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[19]);
}

void CSound::PlayBackGroundSound(int nSound, int nChannel)
{
	FMOD_System_PlaySound(pSystem, pSound[nSound], nullptr, 0, &pChannel[nChannel]);
}

void CSound::SoundStop(int nChannel)
{
	FMOD_Channel_Stop(pChannel[nChannel]);
}

void CSound::SoundPause(int nChannel)
{
	FMOD_Channel_SetPaused(pChannel[nChannel], true);
}

void CSound::SoundResume(int nChannel)
{
	FMOD_Channel_SetPaused(pChannel[nChannel], false);
}

void CSound::SoundRelease()
{
	FMOD_System_Release(pSystem);
}

void CSound::SetVolum(int nChannel, float volume)
{
	FMOD_Channel_SetVolume(pChannel[nChannel], volume);
}
