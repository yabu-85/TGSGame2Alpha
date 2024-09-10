#include "AudioManager.h"
#include "../Engine/Audio.h"
#include "../Engine/Global.h"
#include <cassert>
#include <vector>
#include "../Json/JsonReader.h"

namespace AudioManager
{
	int hSound_[AUDIO_MAX];
	struct AudioData {
		std::string name;
		bool isLoop;
		unsigned short max;
	};
	std::vector<AudioData> sceneTable;

    float gameVolume_;          //Q[ΉΚ

}

void AudioManager::Initialize()
{
    SetAudioData();

    JsonReader::Load("Json/PlayerSetting.json");
    auto& commonSection = JsonReader::GetSection("Common");
    float value = commonSection["gameVolume"];
    SetVolume(value);

}

void AudioManager::Release()
{
	Audio::Release();
}

void AudioManager::SetAudioData()
{
    //enumΜΤΙ
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

    //f[^[h
    for (int i = 0; i < sceneTable.size(); i++) {
        hSound_[i] = Audio::Load(sceneTable[i].name, sceneTable[i].isLoop, sceneTable[i].max);
        assert(hSound_[i] >= 0);
    }

}

void AudioManager::SetVolume(float volume)
{
    //0.0f ` 1.0fΜΝΝΙ§ΐ
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
