#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum AUDIO_TYPE {
	BUTTON_WITHIN = 0,
	BUTTON_PUSH,
	PAUSE_OPEN,
	PAUSE_CLOSE,

	BIG_RELOAD,
	BIG_SHOT,
	BOLT_ACTION,
	BULLET_HIT,
	EMPTY_MAGAZINE,
	SMALL_RELOAD,
	SMALL_SHOT,

	AUDIO_MAX
};

namespace AudioManager {
	void Initialize();
	void Release();
	void SetAudioData();
	void SetVolume(float volume);

	//‰¹‚ð–Â‚ç‚·
	void Play(AUDIO_TYPE id, float volume = 1.0f);
	
	//‰¹‚ðŽ~‚ß‚é
	void Stop(AUDIO_TYPE id);

};