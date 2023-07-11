#pragma once
class CSound;

class SoundManager
{
protected:
	static CSound* m_pSound;
	static SoundManager* instance;
private:
	SoundManager();
	SoundManager(const SoundManager& ref) {}
	SoundManager& operator=(const SoundManager& ref) {}
	~SoundManager();
public:
	static SoundManager& GetInstance()
	{
	if (instance == nullptr) instance = new SoundManager();
	return *instance;
	}
	static void SoundSystem();
	static void PlayBackGroundSound(int32 Scene); //  배경음악 재생용 호출 함수
	static void PlayObjectSound(int32 idx, int32 channel);
	static void SoundStop(int32 nChannel);
	static void SoundPause(int32 nChannel);
	static void SoundResume(int32 nChannel);
	static void SoundRelease();
	static void SetVolum(int nChannel, float volume);
};

