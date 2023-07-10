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
	FMOD_System_CreateSound(pSystem, "Sound/Danger_Background.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[0]);
	FMOD_System_CreateSound(pSystem, "Sound/Danger_Background2.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[1]);

	// 보스
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot1.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[2]);
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot2.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[3]);
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot3.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[4]);
	FMOD_System_CreateSound(pSystem, "Sound/Boss_Shot4.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[5]);

	// 공장직원
	FMOD_System_CreateSound(pSystem, "Sound/Button_Press.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[6]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Down.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[7]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Hit.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[8]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Hit2.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[9]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run1.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[10]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run2.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[11]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Run3.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[12]);	
	FMOD_System_CreateSound(pSystem, "Sound/Character_Scream.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[13]);
	FMOD_System_CreateSound(pSystem, "Sound/Character_Walk.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[14]);

	// 오브젝트
	FMOD_System_CreateSound(pSystem, "Sound/Dangerous_Alarm.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[15]);
	FMOD_System_CreateSound(pSystem, "Sound/Emergency_Door_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[16]);
	FMOD_System_CreateSound(pSystem, "Sound/Generator.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[17]);
	FMOD_System_CreateSound(pSystem, "Sound/Hangar_Door_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[18]);
	FMOD_System_CreateSound(pSystem, "Sound/Shutter_Open.mp3", FMOD_LOOP_NORMAL, nullptr, &pSound[19]);
}

void CSound::MyPlaySound(int nSound, int nChannel)
{
	FMOD_System_PlaySound(pSystem, pSound[nSound], nullptr, 0, &pChannel[nChannel]);
}

void CSound::SoundStop(int nChannel)
{
	FMOD_Channel_Stop(pChannel[nChannel]);
}

void CSound::SoundRelease()
{
	FMOD_System_Release(pSystem);
}

void CSound::SetVolum(int nChannel, float volume)
{
	FMOD_Channel_SetVolume(pChannel[nChannel], volume);
}

//void CSound::SoundPause();
//
//FMOD_SYSTEM* CSound::g_sound_system;
//
//CSound::CSound(const char* path, bool loop) {
//    if (loop) {
//        FMOD_System_CreateSound(g_sound_system, path, FMOD_LOOP_NORMAL, 0, &m_sound);
//    }
//    else {
//        FMOD_System_CreateSound(g_sound_system, path, FMOD_DEFAULT, 0, &m_sound);
//    }
//
//    m_channel = nullptr;
//    m_volume = SOUND_DEFAULT;
//}
//
//CSound::~CSound() {
//    FMOD_Sound_Release(m_sound);
//}
//
//
//int CSound::Init() {
//    FMOD_System_Create(&g_sound_system,64);
//    FMOD_System_Init(g_sound_system, 64, FMOD_INIT_NORMAL, NULL);
//
//    return 0;
//}
//
//int CSound::Release() {
//    FMOD_System_Close(g_sound_system);
//    FMOD_System_Release(g_sound_system);
//
//    return 0;
//}
//
//
//int CSound::play() {
//    FMOD_System_PlaySound(g_sound_system, m_sound, NULL, false, &m_channel);
//
//    return 0;
//}
//
//int CSound::pause() {
//    FMOD_Channel_SetPaused(m_channel, true);
//
//    return 0;
//}
//
//int CSound::resume() {
//    FMOD_Channel_SetPaused(m_channel, false);
//
//    return 0;
//}
//
//int CSound::stop() {
//    FMOD_Channel_Stop(m_channel);
//
//    return 0;
//}
//
//int CSound::volumeUp() {
//    if (m_volume < SOUND_MAX) {
//        m_volume += SOUND_WEIGHT;
//    }
//
//    FMOD_Channel_SetVolume(m_channel, m_volume);
//
//    return 0;
//}
//
//int CSound::volumeDown() {
//    if (m_volume > SOUND_MIN) {
//        m_volume -= SOUND_WEIGHT;
//    }
//
//    FMOD_Channel_SetVolume(m_channel, m_volume);
//
//    return 0;
//}
//
//
//int CSound::Update() {
//    FMOD_Channel_IsPlaying(m_channel, &m_bool);
//
//    if (m_bool) {
//        FMOD_System_Update(g_sound_system);
//    }
//
//    return 0;
//}
