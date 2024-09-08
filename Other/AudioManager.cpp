#include "AudioManager.h"
#include "../Engine/Audio.h"
#include "../Engine/Global.h"
#include <cassert>
#include <vector>

namespace AudioManager
{
	int hSound_[AUDIO_MAX];
	struct AudioData {
		std::string name;
		bool isLoop;
		unsigned short max;
	};
	std::vector<AudioData> sceneTable;

    float gameVolume_;          //ゲーム音量

}

void AudioManager::Initialize()
{
    SetAudioData();
    SetVolume(1.0f);

}

void AudioManager::Release()
{
	Audio::Release();
}

void AudioManager::SetAudioData()
{
    //enumの順番に
    sceneTable = {
        {"Sound/Menu/ButtonWithin.wav", false, 5},
        {"Sound/Menu/ButtonPush.wav", false, 3},
        {"Sound/Menu/PauseOpen.wav", false, 2},
        {"Sound/Menu/PauseClose.wav", false, 2},

        {"Sound/Gun/BigReload.wav", false, 3},
        {"Sound/Gun/BitShot.wav", false, 3},
        {"Sound/Gun/BoltAction.wav", false, 3},
        {"Sound/Gun/BulletHit.wav", false, 3},
        {"Sound/Gun/EmptyMagazine.wav", false, 3},
        {"Sound/Gun/SmallReload.wav", false, 3},
        {"Sound/Gun/SmallShot.wav", false, 10},
    };

    //データロード
    for (int i = 0; i < sceneTable.size(); i++) {
        hSound_[i] = Audio::Load(sceneTable[i].name, sceneTable[i].isLoop, sceneTable[i].max);
        assert(hSound_[i] >= 0);
    }

}

void AudioManager::SetVolume(float volume)
{
    //0.0f ～ 1.0fの範囲に制限
    if (volume < 0.0f)      volume = 0.0f;
    else if (volume >= 1.0f) volume = 1.0f;

    gameVolume_ = volume;
}

void AudioManager::Play(AUDIO_TYPE id, float volume)
{
    Audio::Play(hSound_[(int)id], volume* gameVolume_);
}

void AudioManager::Stop(AUDIO_TYPE id)
{
    Audio::Stop(hSound_[id]);
}
