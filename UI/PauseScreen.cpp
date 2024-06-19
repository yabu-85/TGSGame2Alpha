#include "PauseScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"

PauseUIManager::PauseUIManager() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	pauseTrans_.position_ = { 0.0f, 0.0f, 1.0f };

	AddUi("ReturnGame", UI_BUTTON, XMFLOAT2(0.0f, 0.1f), XMFLOAT2(1.5f, 1.0f), [this]() { true; });
	AddUi("ReturnToTitle", UI_BUTTON, XMFLOAT2(0.0f, -0.2f), XMFLOAT2(1.6f, 1.0f), [this]() { true; });
	AddUi("Exit", UI_BUTTON, XMFLOAT2(0.0f, -0.5f), [this]() { true; });

}

PauseUIManager::~PauseUIManager()
{
}

void PauseUIManager::Draw()
{
	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[1], 150);
	Image::Draw(hPict_[1]);

	Image::SetTransform(hPict_[0], pauseTrans_);
	Image::Draw(hPict_[0]);

	UIManager::Draw();
}