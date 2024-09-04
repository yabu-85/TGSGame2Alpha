#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum AUDIO_TYPE {
	BUTTON_WITHIN = 0,
	BUTTON_PUSH,
	PAUSE_OPEN,
	PAUSE_CLOSE,
	BULLET_HIT1,
};

namespace AudioManager {
	void Initialize();
	void Release();
	void SetAudioData();

	//‰¹‚ð–Â‚ç‚·
	void Play(AUDIO_TYPE id, float volume = 1.0f);
	
};